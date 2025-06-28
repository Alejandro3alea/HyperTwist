#include "AudioMgr.h"
#include "Audio.h"
#include "Resources/ResourceMgr.h"
#include "Misc/Requires.h"
#include "Misc/DataTypes.h"

#include <SDL3_mixer/SDL_mixer.h>

AudioManager* AudioManager::mpInstance;

void AudioManager::Initialize(int sampleRate, int channels)
{
    Requires(SDL_InitSubSystem(SDL_INIT_AUDIO), "Fail initializing SDL_AUDIO subsystem: " + std::string(SDL_GetError()));

    MIX_InitFlags mixerFlags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG;
    Requires(Mix_Init(mixerFlags) == mixerFlags, "Fail initializing SDL_mixer: " + std::string(SDL_GetError()));

    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = sampleRate;
    desiredSpec.format = MIX_DEFAULT_FORMAT;
    desiredSpec.channels = channels;
    Requires(Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desiredSpec), "Fail trying to open the default audio device: " + std::string(SDL_GetError()));

    Mix_AllocateChannels(32);
}

void AudioManager::Shutdown()
{
    //SDL_CloseAudioDevice(mDeviceID);
}

Audio *AudioManager::LoadSound(const std::string &filePath)
{
    Resource<Audio>* sound = ResourceMgr->Load<Audio>(filePath, AudioType::SFX);
    return sound->get();
}

Audio *AudioManager::LoadMusic(const std::string &filePath)
{
    Resource<Audio>* music = ResourceMgr->Load<Audio>(filePath, AudioType::BGM);
    return music->get();
}
