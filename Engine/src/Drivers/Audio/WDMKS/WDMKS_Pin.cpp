#include "WDMKS_Pin.h"
#include "WDMKS_Filter.h"
#include "WDMKS_Exception.h"
#include "WDMKS_AudioDriver.h"
#include "WDMKS_Common.h"
#include "Misc/ColorConsole.h"
#include "Misc/Requires.h"

WDMKS_Pin::WDMKS_Pin(const WDMKS_Filter& parentFilter, i32 pinId) : mParentFilter(parentFilter), mId(pinId)
{
	HANDLE parentHandle = mParentFilter.GetHandle();

	/* Get the COMMUNICATION property */
	{
		KSPIN_COMMUNICATION communication;
		if (!WDMKS_GetPinPropertySimple(parentHandle, pinId, &KSPROPSETID_Pin, KSPROPERTY_PIN_COMMUNICATION, &communication, sizeof(KSPIN_COMMUNICATION)))
		{
			throw WDMKS_Exception("KSPROPERTY_PIN_COMMUNICATION Property retrieval error.");
		}

		if (communication != KSPIN_COMMUNICATION_SINK && communication != KSPIN_COMMUNICATION_BOTH)
		{
			PrintDebug("[AUDIO] - Not an audio output device.");
			throw WDMKS_Exception("Not an audio output device.");
		}
	}

	/* Get dataflow information */
	{
		KSPIN_DATAFLOW dataFlow;
		if (!WDMKS_GetPinPropertySimple(parentHandle, pinId, &KSPROPSETID_Pin, KSPROPERTY_PIN_DATAFLOW, &dataFlow, sizeof(KSPIN_DATAFLOW)))
		{
			throw WDMKS_Exception("KSPROPERTY_PIN_DATAFLOW Property retrieval error.");
		}
		else if (dataFlow != KSPIN_DATAFLOW_IN)
		{
			throw WDMKS_Exception("KSPROPERTY_PIN_DATAFLOW != KSPIN_DATAFLOW_IN");
		}
	}

	/* Get the INTERFACE property list */
	{
		KSMULTIPLE_ITEM* interfaces = nullptr;
		if (!WDMKS_GetPinPropertyMulti(parentHandle, pinId, &KSPROPSETID_Pin, KSPROPERTY_PIN_INTERFACES, &interfaces))
		{
			throw WDMKS_Exception("KSPROPERTY_PIN_INTERFACES Property retrieval error.");
		}

		if (!HasStandardStreaming(interfaces))
		{
			free(interfaces);
			throw WDMKS_Exception("No standard streaming interface found.");
		}

		free(interfaces);
	}

	/* Get the MEDIUM properties list */
	{
		KSMULTIPLE_ITEM* mediums = nullptr;
		if (!WDMKS_GetPinPropertyMulti(parentHandle, pinId, &KSPROPSETID_Pin, KSPROPERTY_PIN_MEDIUMS, &mediums))
		{
			throw WDMKS_Exception("KSPROPERTY_PIN_MEDIUMS Property retrieval error.");
		}

		if (!HasStandardDevio(mediums))
		{
			free(mediums);
			throw WDMKS_Exception("No STANDARD_DEVIO medium found.");
		}

		free(mediums);
	}

	/* Get DATARANGEs */
	KSMULTIPLE_ITEM* dataRangesItem;
	if (!WDMKS_GetPinPropertyMulti(parentHandle, pinId, &KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, &dataRangesItem))
	{
		throw WDMKS_Exception("KSPROPERTY_PIN_DATARANGES Property retrieval error.");
	}

	KSDATARANGE* dataRanges = (KSDATARANGE*)(dataRangesItem + 1);

	/* Find audio DATARANGEs */
	{
		KSDATARANGE* dataRange = dataRanges;
		for (unsigned i = 0; i < dataRangesItem->Count; i++, dataRange = (KSDATARANGE*)(((char*)dataRange) + dataRange->FormatSize))
		{
			if (memcmp(&dataRange->MajorFormat, &KSDATAFORMAT_TYPE_AUDIO, sizeof(GUID)) &&
				memcmp(&dataRange->MajorFormat, &KSDATAFORMAT_TYPE_WILDCARD, sizeof(GUID)))
				continue;

			if (memcmp(&dataRange->SubFormat, &KSDATAFORMAT_SUBTYPE_PCM, sizeof(GUID)) &&
				memcmp(&dataRange->SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(GUID)) &&
				memcmp(&dataRange->SubFormat, &KSDATAFORMAT_SUBTYPE_WILDCARD, sizeof(GUID)))
				continue;

			if (memcmp(&dataRange->Specifier, &KSDATAFORMAT_SPECIFIER_WILDCARD, sizeof(GUID)) &&
				memcmp(&dataRange->Specifier, &KSDATAFORMAT_SPECIFIER_WAVEFORMATEX, sizeof(GUID)))
				continue;

			const KSDATARANGE_AUDIO* pDataRangeAudio = reinterpret_cast<KSDATARANGE_AUDIO*>(dataRange);
			mDataRanges.push_back(*pDataRangeAudio);
		}
	}
	free(dataRangesItem);
	dataRangesItem = nullptr;

	if (mDataRanges.size() == 0)
	{
		throw WDMKS_Exception("Pin has no supported audio data ranges.");
	}
}

WDMKS_Pin::~WDMKS_Pin()
{
	Close();
}

bool WDMKS_Pin::Instantiate(const WAVEFORMATEX* format)
{
	if (!IsFormatSupported(format))
	{
		PrintWarning("[AUDIO] - format not supported");
		return false;
	}

	KSPIN_CONNECT* pinConnect = MakeFormat(format);
	DWORD result = WDMKS_AudioDriver::CreatePin(mParentFilter.GetHandle(), pinConnect, GENERIC_WRITE | GENERIC_READ, &mHandle);
	free(pinConnect);
	if (result != ERROR_SUCCESS)
	{
		PrintWarning("[AUDIO] - Error in WDMKS_AudioDriver::CreatePin function.");
		mHandle = nullptr;
		return false;
	}

	return true;
}

void WDMKS_Pin::Close()
{
	if (mHandle == nullptr)
		return;
	SetState(KSSTATE_PAUSE);
	SetState(KSSTATE_STOP);
	CloseHandle(mHandle);
	mHandle = nullptr;
}

bool WDMKS_Pin::SetState(KSSTATE state)
{
	Requires(mHandle != nullptr, "[AUDIO] - Setting state to uninitialized handle.");
	return WDMKS_SetPropertySimple(mHandle, &KSPROPSETID_Connection, KSPROPERTY_CONNECTION_STATE, &state, sizeof(state), nullptr, 0);
}

KSPIN_CONNECT* WDMKS_Pin::MakeFormat(const WAVEFORMATEX* pFormat) const
{
	if (pFormat == nullptr)
	{
		PrintWarning("[AUDIO] - Passed format is nullptr.");
		return nullptr;
	}

	ULONG waveFormatExSize = sizeof(WAVEFORMATEX) + pFormat->cbSize;
	ULONG dataFormatSize = sizeof(KSDATAFORMAT) + waveFormatExSize;
	ULONG totalSize = sizeof(KSPIN_CONNECT) + dataFormatSize;

	KSPIN_CONNECT* pinConnect = reinterpret_cast<KSPIN_CONNECT*>(malloc(totalSize));
	Requires(pinConnect != nullptr, "[AUDIO] - malloc request failed.");

	memset(pinConnect, 0, totalSize);
	pinConnect->PinId = mId;
	pinConnect->Interface.Set = KSINTERFACESETID_Standard;
	pinConnect->Interface.Id = KSINTERFACE_STANDARD_STREAMING;
	pinConnect->Interface.Flags = 0;
	pinConnect->Medium.Set = KSMEDIUMSETID_Standard;
	pinConnect->Medium.Id = KSMEDIUM_TYPE_ANYINSTANCE;
	pinConnect->Medium.Flags = 0;
	pinConnect->PinToHandle = nullptr;
	pinConnect->Priority.PriorityClass = KSPRIORITY_NORMAL;
	pinConnect->Priority.PrioritySubClass = 1;

	KSDATAFORMAT_WAVEFORMATEX* ksDataFormatWfx = (KSDATAFORMAT_WAVEFORMATEX*)(pinConnect + 1);
	ksDataFormatWfx->DataFormat.Flags = 0;
	ksDataFormatWfx->DataFormat.Reserved = 0;
	ksDataFormatWfx->DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
	ksDataFormatWfx->DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	ksDataFormatWfx->DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;
	ksDataFormatWfx->DataFormat.FormatSize = dataFormatSize;

	memcpy(&ksDataFormatWfx->WaveFormatEx, pFormat, waveFormatExSize);
	ksDataFormatWfx->DataFormat.SampleSize = (unsigned short)(pFormat->nChannels * (pFormat->wBitsPerSample / 8));
	return pinConnect;
}

bool WDMKS_Pin::HasStandardStreaming(KSMULTIPLE_ITEM* interfaces) const
{
	const KSIDENTIFIER* identifier = (KSIDENTIFIER*)&interfaces[1];
	for (unsigned i = 0; i < interfaces->Count; i++)
		if (!memcmp(&identifier[i].Set, &KSINTERFACESETID_Standard, sizeof(GUID)) && identifier[i].Id == KSINTERFACE_STANDARD_STREAMING)
			return true;

	return false;
}

bool WDMKS_Pin::HasStandardDevio(KSMULTIPLE_ITEM* mediums) const
{
	const KSIDENTIFIER* identifier = (KSIDENTIFIER*)&mediums[1];
	for (unsigned i = 0; i < mediums->Count; i++)
		if (!memcmp(&identifier[i].Set, &KSMEDIUMSETID_Standard, sizeof(GUID)) && identifier[i].Id == KSMEDIUM_STANDARD_DEVIO)
			return true;

	return false;
}

bool WDMKS_Pin::IsFormatSupported(const WAVEFORMATEX* pFormat) const
{
	GUID guid = { DEFINE_WAVEFORMATEX_GUID(pFormat->wFormatTag) };

	if (pFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		guid = ((WAVEFORMATEXTENSIBLE*)pFormat)->SubFormat;

	for (const KSDATARANGE_AUDIO& dataRangeAudio : mDataRanges)
	{
		/* This is an audio or wildcard datarange... */
		if (memcmp(&dataRangeAudio.DataRange.SubFormat, &KSDATAFORMAT_SUBTYPE_WILDCARD, sizeof(GUID)) &&
			memcmp(&dataRangeAudio.DataRange.SubFormat, &guid, sizeof(GUID)))
			continue;

		// TODO
		if (dataRangeAudio.MaximumChannels != (ULONG)-1 && dataRangeAudio.MaximumChannels < pFormat->nChannels)
			continue;
		if (pFormat->wBitsPerSample < dataRangeAudio.MinimumBitsPerSample || pFormat->wBitsPerSample > dataRangeAudio.MaximumBitsPerSample)
			continue;
		if (pFormat->nSamplesPerSec < dataRangeAudio.MinimumSampleFrequency || pFormat->nSamplesPerSec > dataRangeAudio.MaximumSampleFrequency)
			continue;

		return true;
	}

	return false;
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