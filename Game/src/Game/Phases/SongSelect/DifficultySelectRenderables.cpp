#include "DifficultySelectRenderables.h"

#include <Utils/GameUtils.h>

DifficultySelectChartRenderables::DifficultySelectChartRenderables(const ChartDifficulty& category, uint8_t level)
{
	mBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mDifficulty.SetFont("data/engine/fonts/Rubik.ttf");
	mDifficulty.SetText("Challenge");
	mLevel.SetFont("data/engine/fonts/Rubik.ttf");
	mLevel.SetText("17");
	mGrade.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mFullComboIcon.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
}

DifficultySelectRenderables::DifficultySelectRenderables(Song* song) : mTable("data/UI/DifficultySelect.json")
{
	SetTextures(song);
	SetPositions();
}

void DifficultySelectRenderables::UpdateSelectorPositions(const std::array<int8_t, 2>& selectorIndices)
{

}

void DifficultySelectRenderables::Show()
{
	mSongThumb.mbIsVisible = true;
	mSongInfoBG.mbIsVisible = true;
	mSongInfoTitle.mbIsVisible = true;
	mSongInfoArtist.mbIsVisible = true;
	mSongInfoBPM.mbIsVisible = true;

	mP1ScoreBG.mbIsVisible = true;
	mP1ScoreTitle.mbIsVisible = true;
	mP1Score.mbIsVisible = true;
	mP2ScoreBG.mbIsVisible = true;
	mP2ScoreTitle.mbIsVisible = true;
	mP2Score.mbIsVisible = true;

	mP1StepArtistBG.mbIsVisible = true;
	mP1StepArtistTitle.mbIsVisible = true;
	mP1StepArtist.mbIsVisible = true;
	mP2StepArtistBG.mbIsVisible = true;
	mP2StepArtistTitle.mbIsVisible = true;
	mP2StepArtist.mbIsVisible = true;

	mChartRenderables;

	mArrowUp.mbIsVisible = true;
	mArrowDown.mbIsVisible = true;
}

void DifficultySelectRenderables::Hide()
{
	mSongThumb.mbIsVisible = false;
	mSongInfoBG.mbIsVisible = false;
	mSongInfoTitle.mbIsVisible = false;
	mSongInfoArtist.mbIsVisible = false;
	mSongInfoBPM.mbIsVisible = false;

	mP1ScoreBG.mbIsVisible = false;
	mP1ScoreTitle.mbIsVisible = false;
	mP1Score.mbIsVisible = false;
	mP2ScoreBG.mbIsVisible = false;
	mP2ScoreTitle.mbIsVisible = false;
	mP2Score.mbIsVisible = false;

	mP1StepArtistBG.mbIsVisible = false;
	mP1StepArtistTitle.mbIsVisible = false;
	mP1StepArtist.mbIsVisible = false;
	mP2StepArtistBG.mbIsVisible = false;
	mP2StepArtistTitle.mbIsVisible = false;
	mP2StepArtist.mbIsVisible = false;

	mChartRenderables;

	mArrowUp.mbIsVisible = false;
	mArrowDown.mbIsVisible = false;
}

std::shared_ptr<DifficultySelectChartRenderables> DifficultySelectRenderables::CreateNewChartRenderables(
	const ChartDifficulty& category, const uint8_t level, const int8_t idx)
{
	/*std::shared_ptr<DifficultySelectChartRenderables> result = std::make_shared<DifficultySelectChartRenderables>(category, level);

	constexpr float textOffsetY = 12.5f;
	const float pos = GetYPosByIndex(idx);
	result->mBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	result->mBG.transform.pos = glm::vec3(-580.0f, pos, 0.5f);
	result->mBG.transform.scale = glm::vec3(260.0f, 32.5f, 0.1f);

	result->mDifficulty.transform.pos = glm::vec3(-610.0f, pos - textOffsetY, 1.75f);
	result->mDifficulty.transform.scale = glm::vec3(0.6f);
	result->mLevel.transform.pos = glm::vec3(-460.0f, pos - textOffsetY, 1.75f);
	result->mLevel.transform.scale = glm::vec3(0.6f);

	result->mP1Grade.transform.pos = glm::vec3(-760.0f, pos, 1.8f);
	result->mP1Grade.transform.scale = glm::vec3(40.0f);
	result->mP2Grade.transform.pos = glm::vec3(-400.0f, pos, 1.8f);
	result->mP2Grade.transform.scale = glm::vec3(40.0f);

	return result;*/
	//return std::make_shared<DifficultySelectChartRenderables>(category, level);
	return nullptr;
}

void DifficultySelectRenderables::SetTextures(const Song* song)
{
    //mSongInfoBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFadeOneSide.png");
    mSongInfoTitle.SetFont("data/engine/fonts/Rubik.ttf");
    mSongInfoTitle.SetText(song->mTitle);
    mSongInfoArtist.SetFont("data/engine/fonts/Rubik.ttf");
    mSongInfoArtist.SetText(song->mArtist);
	mSongInfoBPM.SetFont("data/engine/fonts/Rubik.ttf");
    mSongInfoBPM.SetText("BPM: " + GameUtils::GetBPMLabel(song->mBPMs));

    mP1ScoreBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP1ScoreTitle.SetText("Score");
    mP1ScoreTitle.SetFont("data/engine/fonts/Rubik.ttf");
    mP1Score.SetFont("data/engine/fonts/Rubik.ttf");
    mP1Score.SetText("-");
    mP2ScoreBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP2ScoreTitle.SetText("Score");
    mP2ScoreTitle.SetFont("data/engine/fonts/Rubik.ttf");
    mP2Score.SetFont("data/engine/fonts/Rubik.ttf");
    mP2Score.SetText("-");

    mArrowUp.SetTexture("data/engine/texture/SongSelect/ArrowDown.png");
    mArrowDown.SetTexture("data/engine/texture/SongSelect/ArrowDown.png");
}

void DifficultySelectRenderables::SetPositions()
{
	SET_TABLE_VALUES(mTable, mSongThumb);
	SET_TABLE_VALUES(mTable, mSongInfoBG);
	SET_TABLE_VALUES(mTable, mSongInfoTitle);
	SET_TABLE_VALUES(mTable, mSongInfoArtist);
	SET_TABLE_VALUES(mTable, mSongInfoBPM);

	SET_TABLE_VALUES(mTable, mP1ScoreBG);
	SET_TABLE_VALUES(mTable, mP1ScoreTitle);
	SET_TABLE_VALUES(mTable, mP1Score);

	SET_TABLE_VALUES(mTable, mP2ScoreBG);
	SET_TABLE_VALUES(mTable, mP2ScoreTitle);
	SET_TABLE_VALUES(mTable, mP2Score);

	SET_TABLE_VALUES(mTable, mArrowDown);
	SET_TABLE_VALUES(mTable, mArrowUp);
	mArrowUp.transform.rotation = 180.0f;
}
