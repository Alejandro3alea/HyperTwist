#pragma once
#include "AudioCue.h"
#include "Game/Song.h"
#include "Misc/Singleton.h"
#include "Resources/Resource.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class AudioManager
{
    Singleton(AudioManager);

  public:
    void Initialize(int sampleRate = 48000, int channels = 2);
    void Shutdown();

    Audio* LoadSound(const std::string& filePath);
    Audio* LoadMusic(const std::string& filePath);

    Resource<Audio>* GetBeatTick() { return mBeatTick; }
    Resource<Audio>* GetNoteTick() { return mNoteTick; }

    f32 GetMusicTime();
    void QueueAudioAfter(const Resource<Audio>& audio, const f32 seconds);
    void QueueSoundAtTimestamp(const Resource<Audio>& sfx, const f32 bgm_timestamp_seconds);

    void StopTimerQueue();
    void ResumeTimerQueue();

    void SetupMusicStart();
    void SetupMusicStop();

  private:
    void AudioCueMusicLoop();
    void AudioCueTimerLoop();

  private:
    Resource<Audio>* mBeatTick;
    Resource<Audio>* mNoteTick;

    // Sounds in sync with the music
    std::priority_queue<AudioCue, std::vector<AudioCue>, AudioCueCompare> mSongSyncCues;
    u64 mMusicStartTick = 0.0f;

    // Sounds not in sync with the music (after a timer)
    std::priority_queue<AudioCue, std::vector<AudioCue>, AudioCueCompare> mTimerCues;
    float mQueueTimer = 0.0f;

    std::mutex mCueMusicMutex;
    std::condition_variable mCueMusicCV;
    std::atomic<bool> mIsCueMusicThreadRunning;
    std::thread mCueSongThread;

    std::mutex mCueTimerMutex;
    std::condition_variable mCueTimerCV;
    std::atomic<bool> mIsCueTimerThreadRunning;
    std::atomic<bool> mIsCueTimerThreadPaused;
    std::thread mCueTimerThread;
};

#define AudioMgr AudioManager::Instance()