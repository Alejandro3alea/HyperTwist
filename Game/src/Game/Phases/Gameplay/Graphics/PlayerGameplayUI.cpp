#include "PlayerGameplayUI.h"
#include "Game/Account.h"
#include "Game/Chart.h"
#include "Utils/GameUtils.h"
#include "Utils/StringUtils.h"

PlayerGameplayUI::PlayerGameplayUI(const Chart* chart, const u8 playerIdx)
{
    mPlayerScoreBG.SetTexture("engine/texture/Gameplay/PlayerScoreBG.png");
    mPlayerScoreBG.SetShader("engine/shaders/NotesUI.shader");
    mPlayerDifficultyBG.SetTexture("engine/texture/Gameplay/PlayerScoreBG.png");
    mPlayerDifficultyBG.SetShader("engine/shaders/NotesUI.shader");

    if (playerIdx == PLAYER_1_IDX)
    {
        mPlayerScoreBG.FlipX();
        mPlayerDifficultyBG.FlipX();

        mTable = std::make_unique<UITable>("UI/GameplayP1.json");
    }
    else
    {
        mTable = std::make_unique<UITable>("UI/GameplayP2.json");
    }

    mStageNumberBG.SetTexture("engine/texture/Gameplay/StageHeader.png");
    mStageNumberBG.SetShader("engine/shaders/NotesUI.shader");

    mStageNumberText.SetText("FINAL");
    mStageLabel.SetText("STAGE");

    const std::string difficultyCategoryLabel = ToUpper(GameUtils::ChartDifficultyToStr((chart->mDifficultyCategory)));
    mPlayerDifficultyText.SetText(difficultyCategoryLabel);
    mPlayerDifficultyNumber.SetText(std::to_string(chart->mDifficultyVal));

    mPlayerScoreText.SetText("1.000.000");

    switch (chart->mDifficultyCategory)
    {
    case ChartDifficulty::Beginner:
        mPlayerDifficultyText.mColor = {1.0f, 0.0f, 1.0f, 1.0f};
        break;
    case ChartDifficulty::Easy:
        mPlayerDifficultyText.mColor = {1.0f, 1.0f, 0.0f, 1.0f};
        break;
    case ChartDifficulty::Medium:
        mPlayerDifficultyText.mColor = {1.0f, 0.0f, 0.0f, 1.0f};
        break;
    case ChartDifficulty::Hard:
        mPlayerDifficultyText.mColor = {0.0f, 1.0f, 0.0f, 1.0f};
        break;
    case ChartDifficulty::Challenge:
        mPlayerDifficultyText.mColor = {1.0f, 0.0f, 1.0f, 1.0f};
        break;
    case ChartDifficulty::Special:
        mPlayerDifficultyText.mColor = {1.0f, 1.0f, 0.0f, 1.0f};
        break;
    default:
        PrintWarning("Chart with unknown difficulty category sent to PlayerGameplayUI.");
    }

    SetPositions();
}

void PlayerGameplayUI::SetPositions()
{
    UITable& table = *mTable.get();
    SET_TABLE_VALUES(table, mPlayerScoreBG);
    SET_TABLE_VALUES(table, mPlayerScoreText);
    SET_TABLE_VALUES(table, mPlayerDifficultyBG);
    SET_TABLE_VALUES(table, mPlayerDifficultyText);
    SET_TABLE_VALUES(table, mPlayerDifficultyNumber);

    SET_TABLE_VALUES(table, mStageNumberBG);
    SET_TABLE_VALUES(table, mStageLabel);
    SET_TABLE_VALUES(table, mStageNumberText);

    SET_TABLE_VALUES(table, mLifeMeterBG);
    SET_TABLE_VALUES(table, mLifeMeterBase);
    SET_TABLE_VALUES(table, mLifeMeter);
}
