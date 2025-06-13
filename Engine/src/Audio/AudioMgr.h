#pragma once
#include "Misc/Singleton.h"
#include "Audio.h"

#include "SDL2/SDL_audio.h"

#include <mutex>

class AudioManager 
{
    Singleton(AudioManager);

public:
    void Initialize(int sampleRate = 48000, int channels = 2, int samplesPerBuffer = 256);
    void Shutdown();

    void RegisterAudio(Audio* audio);
    void UnregisterAudio(Audio* audio);

    // The SDL callback function (static wrapper).
    static void SDLAudioCallback(void* userdata, uint8_t* stream, int len);
    // Mix active audio data into the output buffer.
    void MixAudio(uint8_t* stream, int len);

private:
    SDL_AudioDeviceID mDeviceID;
    std::mutex mAudioMutex;
    std::vector<Audio*> mActiveAudios;
};

#define AudioMgr AudioManager::Instance()