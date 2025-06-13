#include "WDMKS_AudioDriver.h"
#include "WDMKS_Exception.h"
#include "Misc/Requires.h"
#include "Math/MathUtils.h"

#include <vector>

#ifdef _WIN32
#define _INC_MMREG
#include <setupapi.h>
#include <ksmedia.h>
#endif

#include <algorithm>

// Driver Pin Creation function pointer declaration
WDMKS_AudioDriver::KsCreatePinFunc CreatePin = nullptr;


static void MapChannels(const int16_t* pIn, int16_t* pOut, int inChannels, int outChannels, int frames, const int* channelMap)
{
	for (size_t i = 0; i < frames; ++i)
	{
		for (size_t j = 0; j < outChannels; ++j)
		{
			if (channelMap[j] == -1)
				pOut[j] = 0;
			else if (channelMap[j] == -2)
			{
				int iSum = 0;
				for (size_t k = 0; k < inChannels; ++k)
					iSum += pIn[k];
				iSum /= inChannels;
				pOut[j] = iSum;
			}
			else
				pOut[j] = pIn[channelMap[j]];
		}
		pOut += outChannels;
		pIn += inChannels;
	}
}

static void MapChannels(const i16* pIn, i16* pOut, const i32 inChannels, const i32 outChannels, const i32 frames)
{
	constexpr i32 i1ChannelMap[] = { -2 };
	constexpr i32 i4ChannelMap[] = { 0, 1, 0, 1 };
	constexpr i32 i5_1ChannelMap[] = { 0, 1, -1, -2, 0, 1 };
	constexpr i32 i7_1ChannelMap[] = { 0, 1, -1, -2, 0, 1, 0, 1 };
	const i32* channelMap;
	switch (outChannels)
	{
	case 1: channelMap = i1ChannelMap; break; // KSAUDIO_SPEAKER_MONO
	case 4: channelMap = i4ChannelMap; break; // KSAUDIO_SPEAKER_QUAD
	case 6: channelMap = i5_1ChannelMap; break; // KSAUDIO_SPEAKER_5POINT1_SURROUND
	case 8: channelMap = i7_1ChannelMap; break; // KSAUDIO_SPEAKER_7POINT1_SURROUND
	default: PrintError("[AUDIO] - Invalid outChannelIdx: " + std::to_string(outChannels));
	}
	MapChannels(pIn, pOut, inChannels, outChannels, frames, channelMap);
}

static void MapSampleFormatFromInt16(const i16* pIn, void* pOut, const i32 samples, const DeviceSampleFormat format)
{
	switch (format)
	{
	case DeviceSampleFormat::eFloat32: // untested
	{
		f32* outBuffer = reinterpret_cast<f32*>(pOut);
		for (i32 i = 0; i < samples; ++i)
			outBuffer[i] = Math::Remap(static_cast<f32>(pIn[i]), -32768.0f, +32767.0f, -1.0f, +1.0f); // [-32768, 32767] -> [-1,+1]
		break;
	}
	case DeviceSampleFormat::eInt24:
	{
		const u8* inBytes = reinterpret_cast<const u8*>(pIn);
		u8* outBuffer = reinterpret_cast<u8*>(pOut);
		for (int i = 0; i < samples; ++i)
		{
			*outBuffer++ = 0;
			*outBuffer++ = *inBytes++;
			*outBuffer++ = *inBytes++;
		}
		break;
	}
	case DeviceSampleFormat::eInt32:
	{
		i16* outBuffer = reinterpret_cast<i16*>(pOut);
		for (int i = 0; i < samples; ++i)
		{
			*outBuffer++ = 0;
			*outBuffer++ = *pIn++;
		}
		break;
	}
	}
}

static std::string GetDevicePath(HANDLE hHandle, SP_DEVICE_INTERFACE_DATA* pInterfaceData)
{
	u8 interfaceDetailsArray[sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + (MAX_PATH * sizeof(WCHAR))];
	const i32 sizeInterface = sizeof(interfaceDetailsArray);
	SP_DEVICE_INTERFACE_DETAIL_DATA* devInterfaceDetails = (SP_DEVICE_INTERFACE_DETAIL_DATA*)interfaceDetailsArray;
	devInterfaceDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	SP_DEVINFO_DATA devInfoData;
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	devInfoData.Reserved = 0;

	if (!SetupDiGetDeviceInterfaceDetail(hHandle, pInterfaceData, devInterfaceDetails, sizeInterface, nullptr, &devInfoData))
		return std::string();

	return devInterfaceDetails->DevicePath;
}


bool WDMKS_AudioDriver::Initialize()
{
	mKernelStreamingUserDll = LoadLibrary("ksuser.dll");
	Requires(mKernelStreamingUserDll != nullptr, "[AUDIO] - Kernel Streaming dll failed loading.");

	CreatePin = reinterpret_cast<KsCreatePinFunc>(GetProcAddress(mKernelStreamingUserDll, "KsCreatePin"));
	if (CreatePin == nullptr)
	{
		PrintWarning("[AUDIO] - KsCreatePin function retrieval error.");
		FreeLibrary(mKernelStreamingUserDll);
		mKernelStreamingUserDll = nullptr;
		return false;
	}

	std::vector<std::shared_ptr<WDMKS_Filter>> filters = std::move(GetFilterList());
	Requires(filters.empty(), "[AUDIO] - Filter list is empty. No supported audio devices found.");

	mFilter = filters[0];
	filters.resize(1);

	// @TODO: Add sound write ahead Property/Preference to a property/preference system
	constexpr i32 soundWriteAhead = 0;
	constexpr i32 soundPreferredSampleRate = 0;

	if (!mStream.Open(mFilter.get(),
		soundWriteAhead, 
		DeviceSampleFormat::eInt16,
		0, // Don't care
		soundPreferredSampleRate))
	{
		return false;
	}

	SetDecodeBufferSize(2048);
	StartDecodeThread();

	mMixingThread = std::thread(SetupMixerThread, this);

	return true;
}

void WDMKS_AudioDriver::PrintDeviceInfo()
{
	const std::vector<std::shared_ptr<WDMKS_Filter>> filters = GetFilterList();
	uint32_t i = 0;
	for (const std::shared_ptr<WDMKS_Filter>& pFilter : filters)
	{
		const WDMKS_Filter* filter = pFilter.get();
		PrintDebug("Device #" + std::to_string(i++) + filter->mFriendlyName);

		uint32_t j = 0;
		for (const WDMKS_Pin& pin : filter->mPins)
		{
			PrintDebug("  Pin " + std::to_string(j++));
			for (KSDATARANGE_AUDIO const& range : pin.mDataRanges)
			{
				std::string subFormat;
				GUID const& rawSubFormat = range.DataRange.SubFormat;
				if (!memcmp(&rawSubFormat, &KSDATAFORMAT_SUBTYPE_WILDCARD, sizeof(GUID)))
					subFormat = "WILDCARD";
				else if (!memcmp(&rawSubFormat, &KSDATAFORMAT_SUBTYPE_PCM, sizeof(GUID)))
					subFormat = "PCM";
				else if (!memcmp(&rawSubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(GUID)))
					subFormat = "FLOAT";

				std::cout << "     Range: " << range.MaximumChannels << " channels, sample " <<
					range.MinimumBitsPerSample << "-" << range.MaximumBitsPerSample << ", " <<
					range.MinimumSampleFrequency << "-" << range.MaximumSampleFrequency << "hz (" <<
					subFormat << ")" << std::endl;
			}
		}
	}
}

std::vector<std::shared_ptr<WDMKS_Filter>> WDMKS_AudioDriver::GetFilterList() const
{
	std::vector<std::shared_ptr<WDMKS_Filter>> result;
	const GUID* pCategoryGuid = (GUID*)&KSCATEGORY_RENDER;

	HDEVINFO handle = SetupDiGetClassDevs(pCategoryGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	Requires(handle == INVALID_HANDLE_VALUE, "[AUDIO] - SetupDiGetClassDevs returned with error code " + std::to_string(GetLastError()));

	for (i32 deviceId = 0;; deviceId++)
	{
		SP_DEVICE_INTERFACE_DATA interfaceData;
		interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		interfaceData.Reserved = 0;

		// Try to find device
		if (!SetupDiEnumDeviceInterfaces(handle, nullptr, pCategoryGuid, deviceId, &interfaceData))
			break;
		else if (interfaceData.Flags == 0 || (interfaceData.Flags & SPINT_REMOVED))
			continue;

		std::string devicePath = GetDevicePath(handle, &interfaceData);
		if (devicePath.empty())
			continue;

		/* Try to get the friendly name for this interface */
		char friendlyName[MAX_PATH] = "(error)";
		DWORD friendlyNameSize = sizeof(friendlyName);
		HKEY hKey = SetupDiOpenDeviceInterfaceRegKey(handle, &interfaceData, 0, KEY_QUERY_VALUE);
		if (hKey != INVALID_HANDLE_VALUE)
		{
			DWORD type;
			if (RegQueryValueEx(hKey, "FriendlyName", 0, &type, (BYTE*)friendlyName, &friendlyNameSize) != ERROR_SUCCESS)
				strcpy(friendlyName, "(error)");
			RegCloseKey(hKey);
		}

		try
		{
			result.push_back(std::make_shared<WDMKS_Filter>(devicePath, friendlyName));
		}
		catch (const WDMKS_Exception& e)
		{
			PrintWarning(std::string("[AUDIO] - ") + e.what());
		}
	}

	if (handle != nullptr)
		SetupDiDestroyDeviceInfoList(handle);

	return result;
}

int64_t WDMKS_AudioDriver::GetPosition() const
{
	KSAUDIO_POSITION pos;

	Requires(WDMKS_GetPropertySimple(mStream.mPlaybackPin->mHandle, &KSPROPSETID_Audio, KSPROPERTY_AUDIO_POSITION,
		&pos, sizeof(pos), nullptr, 0), "[AUDIO] - Error retrieving Audio property set.");

	pos.PlayOffset /= mStream.mBytesPerOutputSample * mStream.mDeviceOutputChannels;
	return pos.PlayOffset;
}

bool WDMKS_AudioDriver::Fill(const i32 packet)
{
	uint64_t iCurrentFrame = GetPosition();

	Read(mStream.mPackets[packet].Data, mStream.mFramesPerChunk, mLastCursorPos, iCurrentFrame);

	/* Increment m_iLastCursorPos. */
	mLastCursorPos += mStream.mFramesPerChunk;

	/* Submit the buffer. */
	return mStream.SubmitPacket(packet);
}

void WDMKS_AudioDriver::Read(void* data, i32 frames, const i32 lastCursorPos, const i32 currentFrame)
{
	/* If we need conversion, read into a temporary buffer.  Otherwise, read directly
	 * into the target buffer. */
	constexpr int32_t channels = 2;
	if (mStream.mDeviceOutputChannels == channels &&
		mStream.mDeviceSampleFormat == DeviceSampleFormat::eInt16)
	{
		i16* buffer = reinterpret_cast<i16*>(data);
		this->Mix(buffer, frames, lastCursorPos, currentFrame);
		return;
	}

	i16* buffer = reinterpret_cast<i16*>(alloca(frames * channels * sizeof(i16)));
	this->Mix((i16*)buffer, frames, lastCursorPos, currentFrame);

	/* If the device has other than 2 channels, convert. */
	if (mStream.mDeviceOutputChannels != channels)
	{
		i16* tempBuffer = reinterpret_cast<i16*>(alloca(frames * mStream.mBytesPerOutputSample * mStream.mDeviceOutputChannels));
		MapChannels(buffer, tempBuffer, channels, mStream.mDeviceOutputChannels, frames);
		buffer = tempBuffer;
	}

	/* If the device format isn't i16, convert. */
	if (mStream.mDeviceSampleFormat != DeviceSampleFormat::eInt16)
	{
		int iSamples = frames * mStream.mDeviceOutputChannels;
		void* tempBuffer = alloca(iSamples * mStream.mBytesPerOutputSample);
		MapSampleFormatFromInt16(buffer, tempBuffer, iSamples, mStream.mDeviceSampleFormat);
		buffer = reinterpret_cast<int16_t*>(tempBuffer);
	}

	memcpy(data, buffer, frames * mStream.mDeviceOutputChannels * mStream.mBytesPerOutputSample);
}

int WDMKS_AudioDriver::SetupMixerThread(void* p)
{
	reinterpret_cast<WDMKS_AudioDriver*>(p)->MixerThread();
	return 0;
}

void WDMKS_AudioDriver::MixerThread()
{
	/* I don't trust this driver with THREAD_PRIORITY_TIME_CRITICAL just yet. */
	if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
		PrintWarning("[AUDIO] - Failed to set sound thread priority");

	/* Enable priority boosting. */
	SetThreadPriorityBoost(GetCurrentThread(), FALSE);

	Requires(mStream.mPlaybackPin != nullptr, "TODO");

	/* Some drivers (stock USB audio in XP) misbehave if we go from KSSTATE_STOP to
	 * KSSTATE_RUN.  Always transition through KSSTATE_PAUSE. */
	Requires(mStream.mPlaybackPin->SetState(KSSTATE_PAUSE) && mStream.mPlaybackPin->SetState(KSSTATE_RUN), "[AUDIO] - Error setting playback pin to run state.");

	/* Submit initial buffers. */
	for (u32 i = 0; i < mStream.mWriteAheadChunks; ++i)
		Fill(i);

	i32 nextBufferToSend = mStream.mWriteAheadChunks;
	nextBufferToSend %= MAX_CHUNKS;

	uint32_t waitFor = 0;

	while (!mShutdown)
	{
		/* Wait for next output buffer to finish. */
		const HANDLE eventHandles[2] = { mSignal, mStream.mSignal[waitFor].hEvent };
		ULONG waitVal = WaitForMultipleObjects(2, eventHandles, FALSE, 1000);

		if (waitVal == WAIT_FAILED)
		{
			PrintWarning("[AUDIO] - WaitForMultipleObjects returned with WAIT_FAILED code.");
			break;
		}
		if (waitVal == WAIT_TIMEOUT)
			continue;

		if (waitVal == WAIT_OBJECT_0)
		{
			/* Abort event */
			Requires(mShutdown, "[AUDIO] - MixerThread shutdown."); /* Should have been set */
			continue;
		}
		++waitFor;
		waitFor %= MAX_CHUNKS;

		if (!Fill(nextBufferToSend))
		{
			PrintWarning("[AUDIO] - Fill function retrieval error.");
			break;
		}

		++nextBufferToSend;
		nextBufferToSend %= MAX_CHUNKS;
	}

	/* Finished, either normally or aborted */
	mStream.mPlaybackPin->SetState(KSSTATE_PAUSE);
	mStream.mPlaybackPin->SetState(KSSTATE_STOP);
}


/*
 * Based on Stepmania's WDMKS Audio driver implementation, which is based on the PortAudio Windows WDM-KS interface.
 *
 * Copyright (c) 1999-2004 Andrew Baldwin, Ross Bencina, Phil Burk
 * Copyright (c) 2002-2006 Glenn Maynard
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