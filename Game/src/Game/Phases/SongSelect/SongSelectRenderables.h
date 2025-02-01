#pragma once
#include "Graphics/Renderable.h"
#include "Game/Song.h"

struct ChartRenderables
{
	ChartRenderables(const ChartDifficulty& category, const uint8_t level);

	Renderable mBG;
	FontRenderer mDifficulty;
	FontRenderer mLevel;
	FontRenderer mP1Grade;
	FontRenderer mP2Grade;
};

struct SongSelectRenderables
{
	SongSelectRenderables();

	void OnSongChange(Resource<Song>* song);

private:
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

	std::vector<ChartRenderables> mChartRenderables;
	Renderable mP1Selector;
	Renderable mP2Selector;

	Renderable mArrowUp;
	Renderable mArrowDown;
};