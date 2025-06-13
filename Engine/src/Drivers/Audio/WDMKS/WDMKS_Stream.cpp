#include "WDMKS_Stream.h"
#include "Misc/Requires.h"
#include "WDMKS_Common.h"

WDMKS_Stream::WDMKS_Stream()
{
	memset(this, 0, sizeof(*this));
	for (int i = 0; i < MAX_CHUNKS; ++i)
		mSignal[i].hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	mPlaybackPin = nullptr;
}

WDMKS_Stream::~WDMKS_Stream()
{
	// @TODO: Check memleaks
	CloseHandle(mSignal[0].hEvent);
	CloseHandle(mSignal[1].hEvent);
	Close();
}

bool WDMKS_Stream::SubmitPacket(const u32 packetIdx)
{
	KSSTREAM_HEADER* p = &mPackets[packetIdx];
	i32 result = DeviceIoControl(mPlaybackPin->mHandle, IOCTL_KS_WRITE_STREAM, nullptr, 0, p, p->Size, nullptr, &mSignal[packetIdx]);
	Requires(result == 0, "DeviceIoControl");

	DWORD errorCode = GetLastError();
	if (errorCode == ERROR_IO_PENDING)
		return true;

	PrintWarning("[AUDIO] - DeviceIoControl returned with error code " + std::to_string(errorCode));
	return false;
}

bool WDMKS_Stream::Open(WDMKS_Filter* filter,
	const i32 writeAheadFrames,
	DeviceSampleFormat preferredOutputSampleFormat,
	i32 preferredOutputChannels,
	i32 preferredSampleRate)
{
	/* Instantiate the output pin. */
	mPlaybackPin = filter->InstantiateRenderPin(preferredOutputSampleFormat, preferredOutputChannels, preferredSampleRate);

	if (mPlaybackPin == nullptr)
	{
		Close();
		return false;
	}

	mDeviceSampleFormat = preferredOutputSampleFormat;
	mDeviceOutputChannels = preferredOutputChannels;
	mSampleRate = preferredSampleRate;
	mBytesPerOutputSample = GetBytesPerSample(mDeviceSampleFormat);

	i32 frameSize = 1;
	{
		KSALLOCATOR_FRAMING ksaf;
		KSALLOCATOR_FRAMING_EX ksafex;
		if (WDMKS_GetPropertySimple(mPlaybackPin->mHandle, &KSPROPSETID_Connection, KSPROPERTY_CONNECTION_ALLOCATORFRAMING,
			&ksaf, sizeof(ksaf), nullptr, 0))
		{
			frameSize = ksaf.FrameSize;
		}
		else if (WDMKS_GetPropertySimple(mPlaybackPin->mHandle, &KSPROPSETID_Connection, KSPROPERTY_CONNECTION_ALLOCATORFRAMING_EX,
			&ksafex, sizeof(ksafex), nullptr, 0))
		{
			frameSize = ksafex.FramingItem[0].FramingRange.Range.MinFrameSize;
		}
	}

	frameSize /= mBytesPerOutputSample * mDeviceOutputChannels;

	mWriteAheadChunks = 2;

	/* If a writeahead was specified, use it. */
	mFramesPerChunk = writeAheadFrames / mWriteAheadChunks;
	if (mFramesPerChunk == 0)
	{
		mFramesPerChunk = 512 / mWriteAheadChunks;
		mFramesPerChunk = max(mFramesPerChunk, frameSize); // iFrameSize may be 0
	}

	PrintDebug("KS: chunk size: " + std::to_string(mFramesPerChunk) + "; allocator framing: " + std::to_string(frameSize) + " (" + std::to_string(frameSize * 1000 / mSampleRate) + "ms)");
	PrintDebug("KS: " + std::to_string(mSampleRate) + " hz");

	/* Set up chunks. */
	for (i32 i = 0; i < MAX_CHUNKS; ++i)
	{
		// _aligned_malloc( size, 64 )?
		KSSTREAM_HEADER* p = &mPackets[i];

		/* Avoid any FileAlignment problems by using VirtualAlloc, which is always page aligned. */
		p->Data = reinterpret_cast<i8*>(VirtualAlloc(nullptr, mFramesPerChunk * mBytesPerOutputSample * mDeviceOutputChannels, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
		Requires(p->Data != nullptr, "[AUDIO] - Data is invalid.");
		p->FrameExtent = mFramesPerChunk * mBytesPerOutputSample * mDeviceOutputChannels;
		p->DataUsed = mFramesPerChunk * mBytesPerOutputSample * mDeviceOutputChannels;
		p->Size = sizeof(*p);
		p->PresentationTime.Numerator = 1;
		p->PresentationTime.Denominator = 1;
	}

	return true;
}

void WDMKS_Stream::Close()
{
	if (mPlaybackPin)
		mPlaybackPin->Close();
	mPlaybackPin = nullptr;
	for (size_t i = 0; i < 2; ++i)
	{
		VirtualFree(mPackets[i].Data, 0, MEM_RELEASE);
		mPackets[i].Data = nullptr;
	}
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