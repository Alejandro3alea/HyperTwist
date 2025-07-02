#pragma once
#include "AudioCue.h"
#include "Misc/Singleton.h"
#include "Resources/Resource.h"

#include <string>
#include <queue>
#include <mutex>

struct Audio;


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
    void QueueSound(Audio* sfx, const f32 time);

private:
    void AudioCueLoop();

private:
	Resource<Audio>* mBeatTick;
	Resource<Audio>* mNoteTick;

    u64 mMusicStartTick = 0.0f;
    std::priority_queue<AudioCue, std::vector<AudioCue>, AudioCueCompare> mCues;

    std::mutex mCueMutex;
    std::condition_variable mCueCV;
    std::atomic<bool> mIsCueThreadRunning;
    std::thread mCueThread;
};

#define AudioMgr AudioManager::Instance()