#pragma once
#include "Resources/ResourceMgr.h"
#include "AudioInfo.h"

#include <mutex>

struct Audio
{
    Audio(const std::string& file);

    void Play(const float positionInMeasure);
    void Play();
    void Stop();

    // Hypertwist
    void SetPosition(const float measure);
    void SetVolume(const float val);

    std::vector<float> GetSamples(const size_t numSamples);


    bool IsPlaying() { return mIsPlaying; }

    void Cleanup();

private:
    AudioInfo LoadWav(const std::string& path);
    AudioInfo LoadMp3(const std::string& path);
    AudioInfo LoadOgg(const std::string& path);

private:
    bool mIsPlaying = false;
    float mVolume = 1.0f;
    size_t mCurrSample = 0;
    std::vector<float> mSamples;

    int32_t mSampleRate = 44100;

    std::mutex mMutex;
};

