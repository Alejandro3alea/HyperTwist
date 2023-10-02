#pragma once
#include "Misc/Singleton.h"

#include <vector>

struct Audio;

class AudioManager
{
	Singleton(AudioManager);

	friend struct Audio;

public:
	void Initialize();
	void Shutdown();

	void Update();

private:
	std::vector<Audio*> mAudioSources;
};

#define AudioMgr AudioManager::Instance()