#pragma once
#include "Drivers/Audio/AudioDriver.h"
#include "WDMKS_Filter.h"
#include "WDMKS_Stream.h"

class WDMKS_AudioDriver : public AudioDriver
{
public:
	virtual bool Initialize() override;

	void PrintDeviceInfo();

private:
	std::vector<std::shared_ptr<WDMKS_Filter>> GetFilterList() const;

	int64_t GetPosition() const;
	bool Fill(int packet);
	void Read(void* dData, int frames, int lastCursorPos, int currentFrame);

	void SetupDecodingThread();
	int SetupMixerThread(void* p);
	void MixerThread();

#ifdef _WIN32
public:
	using KsCreatePinFunc = DWORD(WINAPI*) (HANDLE, PKSPIN_CONNECT, ACCESS_MASK, PHANDLE);
	static KsCreatePinFunc CreatePin;

protected:
	HMODULE mKernelStreamingUserDll = nullptr;

private:
	std::shared_ptr<WDMKS_Filter> mFilter;
	WDMKS_Stream mStream;
	size_t mFilterCount;

	// Mixing
	std::thread mMixingThread;
	bool mShutdown = false;
	int32_t mLastCursorPos = 0;
	HANDLE mSignal = CreateEvent(nullptr, FALSE, FALSE, nullptr);
#endif
};


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