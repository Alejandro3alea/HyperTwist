#pragma once
#include "Graphics/Renderable.h"
#include "UI/UITable.h"
#include "Game/Song.h"

struct DifficultySelectChartRenderables
{
	DifficultySelectChartRenderables(const ChartDifficulty& category, u8 level, i8 chartIdx, u8 playerIdx);

	Transform GetTransformByIndex(const std::array<i8, 2> &selectorIndices);

	void Show();
	void Hide();

public:
	Renderable mBG;
	FontRenderer mDifficulty;
	FontRenderer mLevel;
	Renderable mGrade;
	Renderable mFullComboIcon;

private:
	const i8 mChartIdx;
	const u8 mPlayerIdx;
};

struct DifficultySelectRenderables
{
	explicit DifficultySelectRenderables(const Song* song, const std::array<i8, 2>& selectorIndices);

	void UpdateChartPositions(const std::array<i8, 2>& selectorIndices);

	void Show();
	void Hide();

private:
	std::shared_ptr<DifficultySelectChartRenderables> CreateNewChartRenderables(const ChartDifficulty& category, 
		const u8 level, const i8 chartIdx, const u8 playerIdx, const std::array<i8, 2> &selectorIndices);

	void SetTextures(const Song* song);
	void SetPositions();
	void SetChartRenderables(const Song* song, const std::array<i8, 2>& selectorIndices);

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