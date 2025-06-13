#pragma once
#include "Singleton.h"
#include "Misc/DataTypes.h"

#include <chrono>

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

struct Timestamp
{
	explicit Timestamp() : mTime(Clock::now()) {}

	inline void Touch()
	{
		mTime = Clock::now();
	}
	inline f64 Ago()
	{
		std::chrono::duration<f64>(Clock::now() - mTime).count();
	}

private:
	TimePoint mTime;
};


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
	TimePoint mFrameStartTime;
};

#define TimeMgr TimeManager::Instance()

