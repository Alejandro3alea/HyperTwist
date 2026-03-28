#pragma once
#include "Game/Song.h"
#include "Graphics/Renderables/FontRenderer.h"

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