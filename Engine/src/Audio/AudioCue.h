#pragma once
#include "Audio.h"

struct AudioCue
{
    Audio* mAudio;
    f32 mTime;
};

struct AudioCueCompare
{
    bool operator()(const AudioCue& lhs, const AudioCue& rhs)
    {
        return lhs.mTime > rhs.mTime;
    }
};