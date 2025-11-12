#include "Timer.h"

#include <iostream>

TimeManager* TimeManager::mpInstance;

void TimeManager::StartFrame() { mFrameStartTime = Clock::now(); }

void TimeManager::EndFrame()
{
    auto endTime = Clock::now();
    std::chrono::duration<f64> elapsed = endTime - mFrameStartTime;

    // @TODO: FixedDeltaTime
    // Calculate the frame time
    deltaTime = fixedDeltaTime = elapsed.count();

    // Update the start time for the next frame
    mFrameStartTime = endTime;
}
