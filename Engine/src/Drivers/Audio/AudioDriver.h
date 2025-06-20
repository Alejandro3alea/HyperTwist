#include "Drivers/DriverBase.h"
#include "Audio/Audio.h"
#include "Audio/AudioMixBuffer.h"
#include "Misc/DataTypes.h"
#include "Misc/Timer.h"

#include "Stepmania/CircularBuffer.h"

#include <thread>
#include <mutex>

#define MAX_SOUND_COUNT 32
#define SAMPLES_PER_BLOCK 512

class AudioDriver : public DriverBase
{
	AudioDriver();
	virtual ~AudioDriver();

	i32 GetNextAvailableSoundIdx() const;

	/* An Audio object calls this to request to be played.
	 * XXX: define what we should do when it can't be played (eg. out of
	 * channels) */
	void StartMixing(Audio* sound);

	/* An Audio object calls this to request it to stop it.  When this function
	 * returns, snd is no longer valid; ensure no running threads are still
	 * accessing it before returning.  This must handle gracefully the case where
	 * snd was not actually being played, though it may print a warning. */
	void StopMixing(Audio* sound);

	/* Pause or unpause the given sound.  If the sound was stopped (not paused),
	 * return false and do nothing; otherwise return true and pause or unpause
	 * the sound.  Unlike StopMixing, pausing and unpause a sound will not lose
	 * any buffered sound (but will not release any resources associated with
	 * playing the sound, either). */
	bool PauseMixing(Audio* audio, bool stop);

	i64 GetHardwareFrame(Timestamp* timestamp) const;
	virtual i64 GetPosition() const = 0;
	void low_sample_count_workaround();

	/* Sound startup latency--delay between Play() being called and actually
	 * hearing it.  (This isn't necessarily the same as the buffer latency.) */
	virtual f32 GetPlayLatency() const = 0;

	virtual i32 GetSampleRate() const = 0;

protected:
	/* Start the decoding.  This should be called once the hardware is set up and
	 * GetSampleRate will return the correct value. */
	void StartDecodeThread();

	/* Call this before calling StartDecodeThread to set the desired decoding buffer
	 * size.  This is the number of frames that Mix() will try to be able to return
	 * at once.  This should generally be slightly larger than the sound writeahead,
	 * to allow filling the buffer after an underrun.  The default is 4096 frames. */
	void SetDecodeBufferSize(i32 frames);

	/* Override this to set the priority of the decoding thread, which should be above
	 * normal priority but not realtime. */
	virtual void SetupDecodingThread() { }

	/*
	 * Read mixed data.
	 *
	 * pBuf: buffer to read into
	 * iFrames: number of frames (not samples) to read
	 * frameno: frame number at which this sound will be heard
	 * iCurrentFrame: frame number that is currently being heard
	 *
	 * iCurrentFrame is used for handling start timing.
	 *
	 * This function only mixes data; it will not lock any mutexes or do any file access, and
	 * is safe to call from a realtime thread.
	 */
	void Mix(i16* buffer, i32 frames, i64 frameNumber, i64 currentFrame);
	void Mix(f32* buffer, i32 frames, i64 frameNumber, i64 currentFrame);

	void MixDeinterlaced(f32** buffers, i32 channels, i32 frames, i64 frameNumber, i64 currentFrame);

private:
	/* This mutex is used for serializing with the decoder thread.  Locking this mutex
	 * can take a while. */
	std::mutex mMutex;

	/* This mutex locks all sounds[] which are "available".  (Other sound may safely
	 * be accessed, and sounds may be set to available, without locking this.) */
	std::mutex mSoundListMutex;

	/*
	 * Thread safety and state transitions:
	 *
	 * AVAILABLE: The sound is available to play a new sound. The decoding and mixing threads
	 * will not touch a sound in this state.
	 *
	 * BUFFERING: The sound is stopped but StartMixing() is prebuffering. No other threads
	 * will touch a sound that is BUFFERING. This isn't necessary if only the main thread
	 * can call StartMixing().
	 *
	 * STOPPED: The sound is idle, but memory is still allocated for its buffer. Update()
	 * will deallocate memory and the sound will be changed to AVAILABLE.
	 *
	 * PLAYING: The sound is being decoded by the decoding thread, and played by the mixing
	 * thread.  If the decoding thread hits EOF, the decoding thread will change the state
	 * to STOPPING.
	 *
	 * STOPPING: The sound is being played by the mixing thread.  No new data will be decoded.
	 * Once the data buffer is empty (all sound has been played), Update() will change the
	 * sound to HALTING.
	 *
	 * HALTING: The main thread has called StopMixing or the data buffer is empty.  The mixing
	 * thread will flush any remaining buffered data without playing it, and then move the
	 * sound to STOPPED.
	 *
	 * The mixing thread operates without any locks.  This can lead to a little overlap.  For
	 * example, if StopMixing() is called, moving the sound from PLAYING to HALTING, the mixing
	 * thread might be in the middle of mixing data.  Although HALTING means "discard buffered
	 * data", some data will still be mixed.  This is OK; the data is valid, and the flush will
	 * happen on the next iteration.
	 *
	 * The only state change made by the decoding thread is on EOF: the state is changed
	 * from PLAYING to STOPPING.  This is done while m_Mutex is held, to prevent
	 * races with other threads.
	 *
	 * The only state change made by the mixing thread is from HALTING to STOPPED.
	 * This is done with no locks; no other thread can take a sound out of the HALTING state.
	 *
	 * Do not allocate or deallocate memory in the mixing thread since allocating memory
	 * involves taking a lock. Instead, push the deallocation to the main thread.
	 */
	struct SoundBlock
	{
		float mBuffer[SAMPLES_PER_BLOCK];
		float* mBufferNext; // beginning of the unread data
		int mFramesInBuffer; // total number of frames at m_BufferNext
		i64 mPosition; // stream frame of m_BufferNext
		SoundBlock() : mBufferNext(mBuffer),
			mFramesInBuffer(0), mPosition(0) {}
	};

	struct Sound
	{
		Sound();
		void Allocate(const i32 frames);
		void Deallocate();

		Audio* mSound = nullptr;
		Timestamp mStartTime;
		CircBuf<SoundBlock> mBuffer;

		bool mPaused = false;

		struct QueuedPosMap
		{
			i32 iFrames;
			i64 iStreamFrame;
			i64 iHardwareFrame;
		};

		CircBuf<QueuedPosMap> mPosMapQueue;

		enum
		{
			AVAILABLE,
			BUFFERING,
			STOPPED,	/* idle */

			/* This state is set by the decoder thread, indicating that the sound has just
			 * reached EOF.  Once the mixing thread finishes flushing buffer, it'll change
			 * to the STOPPING_FINISH state. */
			STOPPING,

			HALTING,	/* stop immediately */
			PLAYING
		} mState;
	};

	/* List of currently playing sounds: XXX no vector */
	std::array<Sound, MAX_SOUND_COUNT> mSounds;

	i64 ClampHardwareFrame(i64 hardwareFrame) const;
	mutable i64 mMaxHardwareFrame = 0;
	mutable i64 mVMaxHardwareFrame = 0;
	mutable i32 mSoundDriverMaxSamples = 0;

	i32 mFramesToBuffer;

	bool mShutdownDecodeThread = false;

	static i32 SetupDecodeThread(void* p);
	void DecodeThread();
	AudioMixBuffer& MixIntoBuffer(i32 frames, i64 frameNumber, i64 currentFrame);
	std::thread mDecodeThread;

	int GetDataForSound(Sound& s);
};


/*
 * Based on Stepmania's Base Audio driver implementation
 * 
 * (c) 2002-2004 Glenn Maynard
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
