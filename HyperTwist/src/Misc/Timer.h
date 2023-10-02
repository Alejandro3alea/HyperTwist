#pragma once
#include "Singleton.h"

#include <chrono>

class TimeManager
{
	Singleton(TimeManager);

public:
	void StartFrame();
	void EndFrame();

public:
	float deltaTime = 0.016667f;
	float fixedDeltaTime = 0.016667f;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
};

#define Time TimeManager::Instance()