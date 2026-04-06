#pragma once
#include "Game/Chart.h"
#include "Graphics/Renderables/FontRenderer.h"
#include "Graphics/Renderables/Renderable.h"
#include "Misc/DataTypes.h"
#include "UI/UITable.h"

struct PlayerGameplayUI
{
    PlayerGameplayUI(const Chart* chart, const u8 playerIdx);

    // private:
    void SetPositions();

  public:
    std::unique_ptr<UITable> mTable;

  private:
    Renderable mPlayerScoreBG;
    FontRenderer mPlayerScoreText;
    Renderable mPlayerDifficultyBG;
    FontRenderer mPlayerDifficultyText;
    FontRenderer mPlayerDifficultyNumber;

    Renderable mStageNumberBG;
    FontRenderer mStageLabel;
    FontRenderer mStageNumberText;

    Renderable mLifeMeterBG;
    Renderable mLifeMeterBase;
    Renderable mLifeMeter;
};