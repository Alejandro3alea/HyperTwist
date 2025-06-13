#include "AudioDriver.h"
#include "Misc/Requires.h"

#define CHANNEL_COUNT 2

AudioDriver::AudioDriver()
{
	SetDecodeBufferSize(4096);

	// @TODO: Add mSoundDriverMaxSamples to a preference chart
	mSoundDriverMaxSamples = 0;
}

AudioDriver::~AudioDriver()
{
	if (mDecodeThread.joinable())
	{
		PrintDebug("[AUDIO] - Shutting down decode thread...");
		mShutdownDecodeThread = true;

		// Wait decode thread to finish
		mDecodeThread.join();
		PrintDebug("[AUDIO] - Decode thread shut down.");
	}
}

i32 AudioDriver::GetNextAvailableSoundIdx() const
{
	for (i32 i = 0; i < mSounds.size(); ++i)
		if (mSounds[i].mState == Sound::AVAILABLE)
			return i;

	return -1;
}

void AudioDriver::StartMixing(Audio* sound)
{
	/* Lock available m_Sounds[], and reserve a slot. */
	mSoundListMutex.lock();

	i32 soundIdx = GetNextAvailableSoundIdx();
	if (soundIdx == -1)
	{
		mSoundListMutex.unlock();
		return;
	}

	Sound& s = mSounds[soundIdx];
	s.mState = Sound::BUFFERING;

	/* We've reserved our slot; we can safely unlock now.  Don't hold onto it longer
	 * than needed, since prebuffering might take some time. */
	mSoundListMutex.unlock();

	s.mAudio = sound;
	// @TODO!!!
	//s.mStartTime = sound->GetStartTime();
	s.mStartTime = Timestamp();
	s.mBuffer.clear();

	/* Initialize the sound buffer. */
	s.Allocate(mFramesToBuffer);

	//	LOG->Trace("StartMixing(%s) (%p)", s.m_pSound->GetLoadedFilePath().c_str(), s.m_pSound );

		/* Prebuffer some frames before changing the sound to PLAYING. */
	while (s.mBuffer.num_writable())
	{
		//		LOG->Trace("StartMixing: (#%i) buffering %i (%i writable) (%p)", i, (int) frames_to_buffer, s.buffer.num_writable(), s.m_pSound );
		i32 wrote = GetDataForSound(s);
		if (wrote < 0)
			break;
	}

	s.mState = Sound::PLAYING;
}

void AudioDriver::StopMixing(Audio* audio)
{
	mMutex.lock();

	/* Find the sound. */
	unsigned i;
	for (i = 0; i < mSounds.size(); ++i)
		if (mSounds[i].mState != Sound::AVAILABLE && mSounds[i].mAudio == audio)
			break;

	if (i == mSounds.size())
	{
		mMutex.unlock();
		PrintDebug("[AUDIO] - Not stopping a sound because it's not playing.");
		return;
	}

	/* If we're already in STOPPED, there's nothing to do. */
	if (mSounds[i].mState == Sound::STOPPED)
	{
		mMutex.unlock();
		PrintDebug("[AUDIO] - Not stopping a sound because it's already in STOPPED");
		return;
	}

	//	LOG->Trace("StopMixing: set %p (%s) to HALTING", m_Sounds[i].m_pSound, m_Sounds[i].m_pSound->GetLoadedFilePath().c_str());

		/* Tell the mixing thread to flush the buffer.  We don't have to worry about
		 * the decoding thread, since we've locked m_Mutex. */
	mSounds[i].mState = Sound::HALTING;

	/* Invalidate the m_pSound pointer to guarantee we don't make any further references to
	 * it.  Once this call returns, the sound may no longer exist. */
	mSounds[i].mAudio = nullptr;
	//	LOG->Trace("end StopMixing");

	mMutex.unlock();

	audio->SoundIsFinishedPlaying();
}

bool AudioDriver::PauseMixing(Audio* sound, bool stop)
{
	std::lock_guard<std::mutex> lock(mMutex);

	/* Find the sound. */
	u32 soundIdx;
	for (soundIdx = 0; soundIdx < mSounds.size(); ++soundIdx)
		if (mSounds[soundIdx].mState != Sound::AVAILABLE && mSounds[soundIdx].mAudio == sound)
			break;

	/* A sound can be paused in PLAYING or STOPPING.  (STOPPING means the sound
	 * has been decoded to the end, and we're waiting for that data to finish, so
	 * externally it looks and acts like PLAYING.) */
	if (soundIdx == mSounds.size() || (mSounds[soundIdx].mState != Sound::PLAYING && mSounds[soundIdx].mState != Sound::STOPPING))
	{
		PrintDebug("[AUDIO] - Not pausing a sound because it's not playing.");
		return false;
	}

	mSounds[soundIdx].mPaused = stop;

	return true;
}

i64 AudioDriver::GetHardwareFrame(Timestamp* timer = nullptr) const
{
	if (timer == nullptr)
		return ClampHardwareFrame(GetPosition());

	/*
	 * We may have unpredictable scheduling delays between updating the timestamp
	 * and reading the sound position.  If we're preempted while doing this and
	 * it may have caused the timestamp to not match the returned time, retry.
	 *
	 * As a failsafe, only allow a few attempts.  If this has to try more than
	 * a few times, then probably we have thread contention that's causing more
	 * severe performance problems, anyway.
	 */
	int tries = 3;
	int64_t positionFrames;
	do
	{
		timer->Touch();
		positionFrames = GetPosition();
	} while (--tries && timer->Ago() > 0.002);

	if (tries == 0)
	{
		static bool logged = false;
		if (!logged)
		{
			logged = true;
			PrintWarning("RageSoundDriver::GetHardwareFrame: too many tries");
		}
	}

	return ClampHardwareFrame(positionFrames);
}

void AudioDriver::low_sample_count_workaround()
{
	if (mSoundDriverMaxSamples != 0)
		GetHardwareFrame(nullptr);
}

void AudioDriver::StartDecodeThread()
{
	Requires(!mDecodeThread.joinable(), "[AUDIO] - Decode thread is already enabled.");

	mDecodeThread = std::thread(SetupDecodeThread, this);
}

void AudioDriver::SetDecodeBufferSize(i32 frames)
{
	Requires(!mDecodeThread.joinable(), "[AUDIO] - Decode thread is already enabled.");

	mFramesToBuffer = frames;
}

void AudioDriver::Mix(i16* buffer, i32 frames, i64 frameNumber, i64 currentFrame)
{
	memset(buffer, 0, frames * CHANNEL_COUNT * sizeof(int16_t));
	MixIntoBuffer(frames, frameNumber, currentFrame).read(buffer);
}

void AudioDriver::Mix(f32* buffer, i32 frames, i64 frameNumber, i64 currentFrame)
{
	memset(buffer, 0, frames * CHANNEL_COUNT * sizeof(float));
	MixIntoBuffer(frames, frameNumber, currentFrame).read(buffer);
}

void AudioDriver::MixDeinterlaced(f32** buffers, i32 channels, i32 frames, i64 frameNumber, i64 currentFrame)
{
	for (int i = 0; i < channels; ++i)
		memset(buffers[i], 0, frames * sizeof(float));
	MixIntoBuffer(frames, frameNumber, currentFrame).read_deinterlace(buffers, channels);
}

AudioDriver::Sound::Sound() : mState(AVAILABLE) {}

void AudioDriver::Sound::Allocate(const i32 frames)
{
	/* Reserve enough blocks in the buffer to hold the buffer.  Add one, to account for
	 * the fact that we may have a partial block due to a previous Mix() call. */
	constexpr i32 framesPerBlock = SAMPLES_PER_BLOCK / CHANNEL_COUNT;
	const i32 blocksToPrebuffer = frames / framesPerBlock;
	mBuffer.reserve(blocksToPrebuffer + 1);
	mPosMapQueue.reserve(32);
}

void AudioDriver::Sound::Deallocate()
{
	mBuffer.reserve(0);
	mPosMapQueue.reserve(0);
}
