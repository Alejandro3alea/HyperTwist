#include "Timer.h"

#include <iostream>

TimeManager* TimeManager::mpInstance;

void TimeManager::StartFrame()
{
    mStartTime = std::chrono::high_resolution_clock::now();
}

void TimeManager::EndFrame()
{
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - mStartTime;

    // @TODO: FixedDeltaTime
    // Calculate the frame time
    deltaTime = fixedDeltaTime = elapsed.count();

    // Update the start time for the next frame
    mStartTime = endTime;
}
