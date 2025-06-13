#include "Timer.h"

#include <iostream>

TimeManager* TimeManager::mpInstance;

void TimeManager::StartFrame()
{
    mFrameStartTime = std::chrono::high_resolution_clock::now();
}

void TimeManager::EndFrame()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - mFrameStartTime;

    // @TODO: FixedDeltaTime
    // Calculate the frame time
    deltaTime = fixedDeltaTime = elapsed.count();

    // Update the start time for the next frame
    mFrameStartTime = endTime;
}
