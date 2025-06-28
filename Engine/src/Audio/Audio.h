#pragma once
#include "Resources/ResourceMgr.h"
#include "Misc/DataTypes.h"
#include "AudioInfo.h"

#include <mutex>

struct Mix_Music;
struct Mix_Chunk;

enum class AudioType
{
    BGM,
    SFX
};

// Audios should be created with AudioMgr using LoadSound/LoadMusic respectively
struct Audio
{
    Audio(const std::string& file, const AudioType audioType = AudioType::BGM);

    void Play(const float positionInMeasure);
    void Play();
    void Stop();

    // Hypertwist
    void SetPosition(const float measure);
    void SetVolume(const float val);

    bool IsPlaying() { return mIsPlaying; }

private:
    Mix_Music* LoadBGM(const std::string& path);
    Mix_Chunk* LoadSFX(const std::string& path);
    
    bool PlayBGM();
    bool PlaySFX();

    void StopBGM();
    void StopSFX();

private:
    const AudioType mType;
    void* mData = nullptr;
    bool mIsPlaying = false;
    bool mIsLooping = false;
    float mVolume = 1.0f;

    // For sounds too
    i32 mSFXChannel = -1;
};

