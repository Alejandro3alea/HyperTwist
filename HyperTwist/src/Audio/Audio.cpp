 #include "Audio.h"
#include "AudioMgr.h"
#include "ResourceMgr.h"
#include "Ensure.h"

Audio::Audio(const std::string& file)
{
    mIsWav = ResourceMgr->GetExtension(file) == "wav";

    if (mIsWav)
    {
        Ensure(SDL_LoadWAV(file.c_str(), &mSpec, &mBuffer, &mLength) != nullptr, std::string("Failed to load audio: ") + SDL_GetError());
        mDeviceID = SDL_OpenAudioDevice(nullptr, 0, &mSpec, nullptr, 0);
        if (mDeviceID == 0)
        {
            SDL_FreeWAV(mBuffer);
            Ensure(false, std::string("Failed to open audio device: ") + SDL_GetError());
        }
    }
    else
    {
        mMusic = Mix_LoadMUS(file.c_str());
        Ensure(mMusic != nullptr, std::string("Failed to load audio: ") + SDL_GetError());
    }

    AudioMgr->mAudioSources.push_back(this);
}

Audio::~Audio()
{

    if (mIsWav)
    {
        SDL_CloseAudioDevice(mDeviceID);
        SDL_FreeWAV(mBuffer);
    }
    else
    {
        Mix_FreeMusic(mMusic);
        Mix_CloseAudio();
    }

    AudioMgr->mAudioSources.erase(std::find(AudioMgr->mAudioSources.begin(), AudioMgr->mAudioSources.end(), this));
}

void Audio::Play()
{
    if (mIsWav)
    {
        SDL_QueueAudio(mDeviceID, mBuffer, mLength);
        SDL_PauseAudioDevice(mDeviceID, 0);
    }
    else
    {
        Ensure(Mix_PlayMusic(mMusic, -1) >= 0, std::string("Failed to play MP3 audio: ") + Mix_GetError());
    }
}