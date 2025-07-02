#include "Audio/Audio.h"
#include "Misc/Requires.h"

#include <SDL3_mixer/SDL_mixer.h>
#include "Audio.h"

Audio::Audio(const std::string& file, const AudioType audioType) : mType(audioType)
{
    switch (mType)
    {
    case AudioType::BGM:
        mData = LoadBGM(file);
        break;
    case AudioType::SFX:
        mData = LoadSFX(file);
        break;
    }
}

void Audio::Play(const float positionInMeasure)
{
    Play();
    SetPosition(positionInMeasure);
}

void Audio::Play()
{
    switch (mType)
    {
    case AudioType::BGM:
        PlayBGM();
        break;
    case AudioType::SFX:        
        PlaySFX();
        break;
    }
    mIsPlaying = true;
}

void Audio::Stop()
{
    switch (mType)
    {
    case AudioType::BGM:
        StopBGM();
        break;
    case AudioType::SFX:        
        StopSFX();
        break;
    }
    mIsPlaying = false;
}


void Audio::SetPosition(const float measure)
{
    if (!Mix_SetMusicPosition(measure))
    {
        PrintError("Mix_SetMusicPosition failed: " + std::string(SDL_GetError()));
    }
}

void Audio::SetVolume(const float val)
{
    mVolume = std::clamp(val, 0.0f, 1.0f);
}

Mix_Music* Audio::LoadBGM(const std::string &path)
{
    Mix_Music* musicData = Mix_LoadMUS(path.c_str());
    if (!musicData)
    {
        PrintError("Mix_LoadMUS failed: " + std::string(SDL_GetError()));
        return nullptr;
    }
    return musicData;
}

Mix_Chunk* Audio::LoadSFX(const std::string &path)
{
    Mix_Chunk* chunkData = Mix_LoadWAV(path.c_str());
    if (!chunkData)
    {
        PrintError("Mix_LoadWAV failed: " + std::string(SDL_GetError()));
        return nullptr;
    }
    return chunkData;
}

bool Audio::PlayBGM()
{
    return Mix_PlayMusic(reinterpret_cast<Mix_Music*>(mData), mIsLooping ? -1 : 0);
}

bool Audio::PlaySFX()
{
    mSFXChannel = Mix_PlayChannel(-1, reinterpret_cast<Mix_Chunk*>(mData), mIsLooping ? -1 : 0);
    return mSFXChannel != -1;
}

void Audio::StopBGM()
{
    Mix_HaltMusic();
}

void Audio::StopSFX()
{
    if (mSFXChannel != -1)
        Mix_HaltChannel(mSFXChannel);
}
