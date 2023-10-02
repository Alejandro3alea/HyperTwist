#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>
#include <fstream>
#include <iostream>
#include <bit>

struct Audio
{
    Audio(const std::string& val);
    ~Audio();

    void Play();

private:
    // MP3 + OGG
    Mix_Music* mMusic = nullptr;

    SDL_AudioSpec mSpec;
    Uint32 mLength;
    Uint8* mBuffer = nullptr;
    SDL_AudioDeviceID mDeviceID;

    bool mIsWav = false;
};