#include "WDMKS_Filter.h"
#include "WDMKS_Exception.h"
#include "Misc/ColorConsole.h"

#ifdef _WIN32
#include <setupapi.h>
#endif

#include <algorithm>


template<typename T, typename U>
void MoveToBeginning(std::vector<T>& v, const U& item)
{
	std::vector<T>::iterator it = find(v.begin(), v.end(), item);
	if (it == v.end())
		return;
	std::vector<T>::iterator next = it;
	++next;
	std::copy_backward(v.begin(), it, next);
	*v.begin() = item;
}

static void FillWFEXT(WAVEFORMATEXTENSIBLE* pwfext, DeviceSampleFormat sampleFormat, int sampleRate, int channelCount)
{
	pwfext->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	pwfext->Format.nChannels = channelCount;
	pwfext->Format.nSamplesPerSec = sampleRate;
	switch (channelCount)
	{
	case 1: pwfext->dwChannelMask = KSAUDIO_SPEAKER_MONO; break;
	case 2: pwfext->dwChannelMask = KSAUDIO_SPEAKER_STEREO; break;
	case 4: pwfext->dwChannelMask = KSAUDIO_SPEAKER_QUAD; break;
	case 6: pwfext->dwChannelMask = KSAUDIO_SPEAKER_5POINT1; break; // or KSAUDIO_SPEAKER_5POINT1_SURROUND
	case 8: pwfext->dwChannelMask = KSAUDIO_SPEAKER_7POINT1_SURROUND; break; // or KSAUDIO_SPEAKER_7POINT1
	}

	switch (sampleFormat)
	{
	case DeviceSampleFormat::eFloat32: pwfext->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; break;
	case DeviceSampleFormat::eInt32: pwfext->SubFormat = KSDATAFORMAT_SUBTYPE_PCM; break;
	case DeviceSampleFormat::eInt24: pwfext->SubFormat = KSDATAFORMAT_SUBTYPE_PCM; break;
	case DeviceSampleFormat::eInt16: pwfext->SubFormat = KSDATAFORMAT_SUBTYPE_PCM; break;
	}

	pwfext->Format.nBlockAlign = GetBytesPerSample(sampleFormat);
	pwfext->Format.wBitsPerSample = pwfext->Format.nBlockAlign * 8;
	pwfext->Format.nBlockAlign *= channelCount;
	pwfext->Samples.wValidBitsPerSample = pwfext->Format.wBitsPerSample;
	pwfext->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	pwfext->Format.nAvgBytesPerSec = pwfext->Format.nSamplesPerSec * pwfext->Format.nBlockAlign;
}


WDMKS_Filter::WDMKS_Filter(const std::string& name, const std::string& friendlyName) : mName(name), mFriendlyName(friendlyName)
{
	// Get the handle of the filter (Device)
	mHandle = CreateFile(mName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);
	if (mHandle != nullptr)
	{
		throw WDMKS_Exception("Filter was not properly created.");
	}

	// Get all possible pins
	int32_t pinCount;
	if (!WDMKS_GetPinPropertySimple(mHandle, 0, &KSPROPSETID_Pin, KSPROPERTY_PIN_CTYPES, &pinCount, sizeof(pinCount)))
	{
		throw WDMKS_Exception("Filter was not properly created.");
	}

	// Create each pin for each possible one
	for (int32_t pinId = 0; pinId < pinCount; pinId++)
	{
		try
		{
			mPins.emplace_back(pinId);
		}
		catch (const WDMKS_Exception& e)
		{
			PrintWarning(std::string("[AUDIO] - ") + e.what());
		}
	}

	if (mPins.empty())
	{
		throw WDMKS_Exception("Filter has no supported audio pins.");
	}
}

WDMKS_Filter::~WDMKS_Filter()
{
	if (mHandle != nullptr)
		CloseHandle(mHandle);
}



WDMKS_Pin* WDMKS_Filter::InstantiateRenderPin(const WAVEFORMATEX* wfex)
{
	for (u32 i = 0; i < mPins.size(); ++i)
	{
		WDMKS_Pin& pin = mPins[i];
		if (pin.Instantiate(wfex))
		{
			return &pin;
		}
	}

	PrintWarning("No pin supports format");
	return nullptr;
}

WDMKS_Pin* WDMKS_Filter::InstantiateRenderPin(DeviceSampleFormat& preferredOutputSampleFormat, i32& preferredOutputChannels, i32& preferredSampleRate)
{
	/*
	 * All Preferred settings are hints, and can be ignored if needed.
	 *
	 * Drivers can advertise DataRanges, and reject formats that fit it.  This is documented
	 * for devices that don't support mono sound; it may happen for other cases.
	 *
	 * Supported channel configurations are 8 (7.1), 6 (5.1), 4, 2 (stereo) and 1 (mono).  Prefer
	 * more channels, since some drivers won't send audio to rear speakers in stereo modes.  Sort
	 * the preferred channel count first.
	 */
	std::vector<i32> channels{ 8, 6, 4, 2, 1 };
	MoveToBeginning(channels, preferredOutputChannels);

	/* Try all sample formats.  Try PreferredOutputSampleFormat first. */
	std::vector<DeviceSampleFormat> sampleFormats{
		DeviceSampleFormat::eInt16,
		DeviceSampleFormat::eInt24,
		DeviceSampleFormat::eInt32,
		DeviceSampleFormat::eFloat32
	};
	MoveToBeginning(sampleFormats, preferredOutputSampleFormat);

	/*
	 * Some hardware may advertise support for 44.1khz, but actually use poor resampling.
	 * Try to use 48khz before 44.1khz, unless iPreferredSampleRate specifically asks
	 * for 44.1khz.
	 *
	 * Try all samplerates listed in the device's DATARANGES.  Sort iSampleRate first,
	 * then 48k, then 44.1k, then higher sample rates first.
	 */
	std::vector<i32> sampleRates;
	{
		for (WDMKS_Pin& pin : mPins)
		{
			for (KSDATARANGE_AUDIO const& range : pin.mDataRanges)
			{
				sampleRates.push_back(range.MinimumSampleFrequency);
				sampleRates.push_back(range.MaximumSampleFrequency);
			}
		}

		if (preferredSampleRate != 0)
			sampleRates.push_back(preferredSampleRate);
		sampleRates.push_back(48000);
		sampleRates.push_back(44100);

		std::sort(sampleRates.begin(), sampleRates.end());
		sampleRates.erase(std::unique(sampleRates.begin(), sampleRates.end()), sampleRates.end());
		std::reverse(sampleRates.begin(), sampleRates.end());

		MoveToBeginning(sampleRates, 44100);
		MoveToBeginning(sampleRates, 48000);
		if (preferredSampleRate != 0)
			MoveToBeginning(sampleRates, preferredSampleRate);
	}

	/* Try WAVE_FORMAT_EXTENSIBLE, then WAVE_FORMAT_PCM. */
	std::vector<bool> aTryPCM = { false, true };

	for (auto bTryPCM = aTryPCM.begin(); bTryPCM != aTryPCM.end(); ++bTryPCM)
	{
		for (int const& iSampleRate : sampleRates)
		{
			for (int const& iChannels : channels)
			{
				for (DeviceSampleFormat& fmt : sampleFormats)
				{
					preferredOutputSampleFormat = fmt;
					preferredOutputChannels = iChannels;
					preferredSampleRate = iSampleRate;

					WAVEFORMATEXTENSIBLE wfx;
					FillWFEXT(&wfx, preferredOutputSampleFormat, preferredSampleRate, preferredOutputChannels);
					if (*bTryPCM)
					{
						/* Try WAVE_FORMAT_PCM instead of WAVE_FORMAT_EXTENSIBLE. */
						wfx.Format.wFormatTag = WAVE_FORMAT_PCM;
						wfx.Format.cbSize = 0;
						wfx.Samples.wValidBitsPerSample = 0;
						wfx.dwChannelMask = 0;
						wfx.SubFormat = GUID_NULL;
					}

					PrintDebug("KS: trying format: X channels: " + std::to_string(preferredOutputChannels) + " samplerate: " +
						std::to_string(preferredSampleRate) + " format: " + std::to_string(wfx.Format.wFormatTag));

					WDMKS_Pin* playbackPin = InstantiateRenderPin(reinterpret_cast<WAVEFORMATEX*>(&wfx));
					if (playbackPin != nullptr)
					{
						PrintDebug("KS: success");
						return playbackPin;
					}
				}
			}
		}
	}

	PrintWarning("No compatible format found");
	return nullptr;
}


/*
 * Based on Stepmania's WDMKS Audio driver implementation, which is based on the PortAudio Windows WDM-KS interface.
 *
 * Copyright (c) 1999-2004 Andrew Baldwin, Ross Bencina, Phil Burk
 * Copyright (c) 2002-2006 Glenn Maynard
 * Copyright (c) 2025 Alejandro Balea
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

 /*
  * The text above constitutes the entire PortAudio license; however,
  * the PortAudio community also makes the following non-binding requests:
  *
  * Any person wishing to distribute modifications to the Software is
  * requested to send the modifications to the original developer so that
  * they can be incorporated into the canonical version. It is also
  * requested that these non-binding requests be included along with the
  * license above.
  */