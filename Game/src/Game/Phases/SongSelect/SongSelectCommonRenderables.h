#pragma once
#include "Game/Song.h"

struct SongSelectCommonRenderables
{
    SongSelectCommonRenderables();

private:
    void SetTextures();
    void SetPositions();

public:
    Renderable mStageBG;
    FontRenderer mStageText;

    Renderable mNote;
};