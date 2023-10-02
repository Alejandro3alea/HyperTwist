#include "AudioMgr.h"
#include "Audio.h"
#include "Ensure.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <glm/glm.hpp>

AudioManager* AudioManager::mpInstance;

void AudioManager::Initialize()
{
	// Initialize SDL Audio
	Ensure(SDL_Init(SDL_INIT_AUDIO) >= 0 && Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0, std::string("SDL initialization failed: ") + SDL_GetError());
}

void AudioManager::Shutdown()
{
	SDL_Quit();
}

void AudioManager::Update()
{
	/*for (auto it : mAudioSources)
		it->Update();*/
}