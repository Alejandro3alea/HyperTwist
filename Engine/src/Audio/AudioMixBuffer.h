#pragma once
#include "Misc/DataTypes.h"

class AudioMixBuffer
{
public:
	AudioMixBuffer() = default;
	~AudioMixBuffer();

	/* Mix the given buffer of samples. */
	void Write(const f32* buffer, u32 size, const i32 sourceStride = 1, const i32 destStride = 1);

	/* Extend the buffer as if write() was called with a buffer of silence. */
	void Extend(const u32 samples);

	void Read(i16* buffer);
	void Read(f32* buffer);
	void ReadDeinterlace(f32** buffers, const i32 channels);
	f32* Read() { return mMixbuffer; }

	u32 Size() const { return mBufferUsedSize; }

	void SetWriteOffset(const i32 offset) { mOffset = offset; }

private:
	f32* mMixbuffer = nullptr;
	u32 mBufferCapacity = 0;
	u32 mBufferUsedSize = 0;
	i32 mOffset = 0;
};

/*
 * Copyright (c) 2002-2004 Glenn Maynard
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
