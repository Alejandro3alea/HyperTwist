#include "AudioMixBuffer.h"

#include <algorithm>

AudioMixBuffer::~AudioMixBuffer()
{
	free(mMixbuffer);
}

void AudioMixBuffer::Write(const f32* buffer, u32 size, const i32 sourceStride, const i32 destStride)
{
	if (size == 0)
		return;

	/* iSize = 3, iDestStride = 2 uses 4 frames.  Don't allocate the stride of the
	 * last sample. */
	Extend(size * destStride - (destStride - 1));

	/* Scale volume and add. */
	f32* destBuf = mMixbuffer + mOffset;

	while (size)
	{
		*destBuf += *buffer;
		buffer += sourceStride;
		destBuf += destStride;
		--size;
	}
}

void AudioMixBuffer::Extend(const u32 samples)
{
	const u32 realsize = samples + mOffset;
	if (mBufferCapacity < realsize)
	{
		mMixbuffer = reinterpret_cast<f32*>(realloc(mMixbuffer, sizeof(f32) * realsize));
		mBufferCapacity = realsize;
	}

	if (mBufferCapacity < realsize)
	{
		memset(mMixbuffer + mBufferUsedSize, 0, (realsize - mBufferUsedSize) * sizeof(f32));
		mBufferCapacity = realsize;
	}
}

void AudioMixBuffer::Read(i16* buffer)
{
	for (unsigned pos = 0; pos < mBufferUsedSize; ++pos)
	{
		float value = mMixbuffer[pos];
		value = std::clamp(value, -1.0f, +1.0f);
		buffer[pos] = lrintf(value * 32767);
	}
	mBufferUsedSize = 0;
}

void AudioMixBuffer::Read(f32* buffer)
{
	memcpy(buffer, mMixbuffer, mBufferUsedSize * sizeof(float));
	mBufferUsedSize = 0;
}

void AudioMixBuffer::ReadDeinterlace(f32** buffers, const i32 channels)
{
	for (unsigned i = 0; i < mBufferUsedSize / channels; ++i)
		for (int ch = 0; ch < channels; ++ch)
			buffers[ch][i] = mMixbuffer[channels * i + ch];
	mBufferUsedSize = 0;
}
}

