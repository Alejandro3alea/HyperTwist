#pragma once
#include "Misc/DataTypes.h"

#ifdef _WIN32
#define _INC_MMREG
#include <Windows.h>
#include <ks.h>
#include <ksmedia.h>
#endif

#include <vector>

struct WDMKS_Filter;

struct WDMKS_Pin
{
	friend class WDMKS_AudioDriver;

	WDMKS_Pin(const WDMKS_Filter& parentFilter, const i32 pinId);
	~WDMKS_Pin();

public:
	bool Instantiate(const WAVEFORMATEX* format);
	void Close();

	bool SetState(KSSTATE state);

private:
	KSPIN_CONNECT* MakeFormat(const WAVEFORMATEX* format) const;

	bool HasStandardStreaming(KSMULTIPLE_ITEM* interfaces) const;
	bool HasStandardDevio(KSMULTIPLE_ITEM* interfaces) const;
	bool IsFormatSupported(const WAVEFORMATEX* format) const;

public:
	HANDLE mHandle = nullptr;
	i32 mId;

	const WDMKS_Filter& mParentFilter;
	std::vector<KSDATARANGE_AUDIO> mDataRanges;
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