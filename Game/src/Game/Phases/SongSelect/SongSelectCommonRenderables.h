#pragma once
#include "Game/Song.h"

struct SongSelectCommonRenderables
{
    SongSelectCommonRenderables();

    void OnSongChange(Song* song);

    void UpdateSelectorPositions(const std::array<int8_t, 2>& selectorIndices);

    void ShowSelectedSongData();
    void HideSelectedSongData();

private:

    void SetTextures();
    void SetPositions();

public:
    Renderable mStageBG;
    FontRenderer mStageText;

    Renderable mNote;
};