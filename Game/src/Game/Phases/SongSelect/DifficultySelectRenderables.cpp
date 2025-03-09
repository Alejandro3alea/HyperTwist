#include "DifficultySelectRenderables.h"

DifficultySelectRenderables::DifficultySelectRenderables(Song* song)
{
}

void DifficultySelectRenderables::UpdateSelectorPositions(const std::array<int8_t, 2>& selectorIndices)
{
}

void DifficultySelectRenderables::Show()
{
}

void DifficultySelectRenderables::Hide()
{
}

void DifficultySelectRenderables::SetTextures()
{
}

void DifficultySelectRenderables::SetPositions()
{
    mSongInfoBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
    mSongInfoTitle.SetFont("data/engine/fonts/Rubik.ttf");
    mSongInfoTitle.SetText("Climax");
    mSongInfoArtist.SetFont("data/engine/fonts/Rubik.ttf");
    mSongInfoArtist.SetText("USAO");

    mP1ScoreBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP1ScoreTitle.SetText("P1  Score");
    mP1ScoreTitle.SetFont("data/engine/fonts/Rubik.ttf");
    mP1Score.SetFont("data/engine/fonts/Rubik.ttf");
    mP1Score.SetText("-");
    mP2ScoreBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP2ScoreTitle.SetText("P2  Score");
    mP2ScoreTitle.SetFont("data/engine/fonts/Rubik.ttf");
    mP2Score.SetFont("data/engine/fonts/Rubik.ttf");
    mP2Score.SetText("-");

    mP1Selector.SetTexture("data/engine/texture/SongSelect/P1 Selector.png");
    mP2Selector.SetTexture("data/engine/texture/SongSelect/P2 Selector.png");

    mArrowUp.SetTexture("data/engine/texture/SongSelect/ArrowDown.png");
    mArrowDown.SetTexture("data/engine/texture/SongSelect/ArrowDown.png");
}
