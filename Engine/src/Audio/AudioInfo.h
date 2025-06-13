#pragma once
#include <vector>

struct AudioInfo
{
    std::vector<int16_t> pcm;
    int32_t sampleRate;
    int32_t channels;
};