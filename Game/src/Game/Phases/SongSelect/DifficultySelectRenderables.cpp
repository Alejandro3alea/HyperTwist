#include "DifficultySelectRenderables.h"

#include <Utils/GameUtils.h>

DifficultySelectRenderables::DifficultySelectRenderables(Song* song)
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

void DifficultySelectRenderables::SetTextures(Song* song)
{
    mSongInfoBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFadeOneSide.png");
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
	// Creepily staring at these values (maybe @TODO for making a UI file IO system?)
	// TODO: Json system for UI elements
	mSongThumb.transform.pos = glm::vec3(-280.0f, 450.0f, 0.5f);
	mSongThumb.transform.scale = glm::vec3(100.0f);
	mSongInfoBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.75f);
	mSongInfoBG.transform.pos = glm::vec3(0.0f, 350.0f, 0.5f);
	mSongInfoBG.transform.scale = glm::vec3(260.0f, 40.0f, 0.1f);
	mSongInfoTitle.transform.pos = glm::vec3(0.0f, 350.0f, 1.75f);
	mSongInfoTitle.transform.scale = glm::vec3(0.7f);
	mSongInfoArtist.transform.pos = glm::vec3(0.0f, 300.0f, 1.5f);
	mSongInfoArtist.transform.scale = glm::vec3(0.35f);
	mSongInfoBPM.transform.pos = glm::vec3(150.0f, 250.0f, 1.5f);
	mSongInfoBPM.transform.scale = glm::vec3(0.35f);

	mP1ScoreBG.transform.pos = glm::vec3(-600.0f, 0.0f, 0.5f);
	mP1ScoreBG.transform.scale = glm::vec3(140.0f, 40.0f, 0.1f);
	mP1ScoreBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	mP1ScoreTitle.transform.pos = glm::vec3(-600.0f, 15.0f, 1.0f);
	mP1ScoreTitle.transform.scale = glm::vec3(0.35f);
	mP1Score.transform.pos = glm::vec3(-600.0f, 0.0f, 1.0f);
	mP1Score.transform.scale = glm::vec3(0.5f);

	mP2ScoreBG.transform.pos = glm::vec3(600.0f, 0.0f, 0.5f);
	mP2ScoreBG.transform.scale = glm::vec3(140.0f, 40.0f, 0.1f);
	mP2ScoreBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	mP2ScoreTitle.transform.pos = glm::vec3(600.0f, 15.0f, 1.0f);
	mP2ScoreTitle.transform.scale = glm::vec3(0.35f);
	mP2Score.transform.pos = glm::vec3(-600.0f, 0.0f, 1.0f);
	mP2Score.transform.scale = glm::vec3(0.5f);

	mArrowDown.transform.pos = glm::vec3(890.0f, -490.0f, 0.01f);
	mArrowDown.transform.scale = glm::vec3(30.0f, 15.0f, 0.1f);
	mArrowUp.transform.pos = glm::vec3(890.0f, 490.0f, 0.01f);
	mArrowUp.transform.scale = glm::vec3(30.0f, 15.0f, 0.1f);
	mArrowUp.transform.rotation = 180.0f;
}
