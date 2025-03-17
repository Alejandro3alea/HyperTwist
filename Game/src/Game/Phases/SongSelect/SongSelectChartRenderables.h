#pragma once
#include "Graphics/Renderable.h"
#include "Game/Song.h"
#include "UI/UITable.h"

struct SongSelectChartRenderables
{
	SongSelectChartRenderables(const ChartDifficulty& category, uint8_t level);

	void Show();
	void Hide();

	Renderable mBG;
	FontRenderer mDifficulty;
	FontRenderer mLevel;
	Renderable mP1Grade;
	Renderable mP2Grade;
};

struct SongSelectRenderables
{
	SongSelectRenderables();

	void OnSongChange(Song* song);

	void UpdateSelectorPositions(const std::array<int8_t, 2>& selectorIndices);

	void Show();
	void Hide();

private:
	std::shared_ptr<SongSelectChartRenderables> CreateNewChartRenderables(const ChartDifficulty& category, uint8_t level, uint8_t idx);

	void UpdateSelectedSongData(Song* song);

	void SetTextures();
	void SetInitialVisibility();

	float GetYPosByIndex(uint8_t idx);

public:
	UITable mTable;

	Renderable mSongThumb;
	Renderable mSongInfoBG;
	FontRenderer mSongInfoTitle;
	FontRenderer mSongInfoArtist;

	Renderable mP1ScoreBG;
	FontRenderer mP1ScoreTitle;
	FontRenderer mP1Score;
	Renderable mP2ScoreBG;
	FontRenderer mP2ScoreTitle;
	FontRenderer mP2Score;

	std::vector<std::shared_ptr<SongSelectChartRenderables>> mChartRenderables;
	Renderable mP1Selector;
	Renderable mP2Selector;

	Renderable mArrowUp;
	Renderable mArrowDown;
};