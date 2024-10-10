#include "PauseTimer.h"
#include "Misc/Timer.h"

PauseTimer::PauseTimer(const float time) : mTime(time)
{
	//Editor->mPauseTimer = *this;
}

void PauseTimer::Update()
{
	mTime -= TimeMgr->deltaTime;
}

void PauseTimer::Reset()
{
	mTime = 0.0f;
}
