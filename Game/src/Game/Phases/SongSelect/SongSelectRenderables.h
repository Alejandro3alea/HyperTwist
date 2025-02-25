#pragma once
#include "Graphics/Renderable.h"
#include "Game/Song.h"

struct ChartRenderables
{
	ChartRenderables(const ChartDifficulty& category, const uint8_t level);

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

	void ShowSelectedSongData();
	void HideSelectedSongData();
	
private:
	std::shared_ptr<ChartRenderables> CreateNewChartRenderables(const ChartDifficulty& category, const uint8_t level, const uint8_t idx);
	
	void UpdateSelectedSongData(Song* song);

	void SetTextures();
	void SetInitialVisibility();

public:
	Renderable mStageBG;
	FontRenderer mStageText;

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

	std::vector<std::shared_ptr<ChartRenderables>> mChartRenderables;
	Renderable mP1Selector;
	Renderable mP2Selector;

	Renderable mArrowUp;
	Renderable mArrowDown;
};