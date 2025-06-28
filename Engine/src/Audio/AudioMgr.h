#pragma once
#include "Misc/Singleton.h"

#include <string>

struct Audio;

class AudioManager 
{
    Singleton(AudioManager);

public:
    void Initialize(int sampleRate = 48000, int channels = 2);
    void Shutdown();

    Audio* LoadSound(const std::string& filePath);
    Audio* LoadMusic(const std::string& filePath);
};

#define AudioMgr AudioManager::Instance()