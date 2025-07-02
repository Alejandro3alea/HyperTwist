#pragma once
#include "Singleton.h"
#include "Misc/DataTypes.h"

#include <chrono>

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

class TimeManager
{
	Singleton(TimeManager);

public:
	void StartFrame();
	void EndFrame();

public:
	f64 deltaTime = 0.016667;
	f64 fixedDeltaTime = 0.016667;

private:
	TimePoint mFrameStartTime;
};

#define TimeMgr TimeManager::Instance()

