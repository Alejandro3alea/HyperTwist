#pragma once
#include "Graphics/Renderable.h"
#include "UI/UITable.h"
#include "Game/Song.h"

struct DifficultySelectChartRenderables
{
	DifficultySelectChartRenderables(const ChartDifficulty& category, uint8_t level);

	Renderable mBG;
	FontRenderer mDifficulty;
	FontRenderer mLevel;
	Renderable mGrade;
	Renderable mFullComboIcon;
};

struct DifficultySelectRenderables
{
	explicit DifficultySelectRenderables(Song* song);

	void UpdateSelectorPositions(const std::array<int8_t, 2>& selectorIndices);

	void Show();
	void Hide();

private:
	std::shared_ptr<DifficultySelectChartRenderables> CreateNewChartRenderables(const ChartDifficulty& category, uint8_t level, int8_t idx);

	void SetTextures(const Song* song);
	void SetPositions();

public:
	UITable mTable;

	Renderable mSongThumb;
	Renderable mSongInfoBG;
	FontRenderer mSongInfoTitle;
	FontRenderer mSongInfoArtist;
	FontRenderer mSongInfoBPM;

	Renderable mP1ScoreBG;
	FontRenderer mP1ScoreTitle;
	FontRenderer mP1Score;
	Renderable mP2ScoreBG;
	FontRenderer mP2ScoreTitle;
	FontRenderer mP2Score;
    
	Renderable mP1StepArtistBG;
	FontRenderer mP1StepArtistTitle;
	FontRenderer mP1StepArtist;
	Renderable mP2StepArtistBG;
	FontRenderer mP2StepArtistTitle;
	FontRenderer mP2StepArtist;

	std::vector<std::shared_ptr<DifficultySelectChartRenderables>> mChartRenderables;

	Renderable mArrowUp;
	Renderable mArrowDown;
};