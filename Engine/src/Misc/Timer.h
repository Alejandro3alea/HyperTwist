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
	double deltaTime = 0.016667f;
	double fixedDeltaTime = 0.016667f;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
};

#define TimeMgr TimeManager::Instance()