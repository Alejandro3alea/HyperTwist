#include "SongSelectRenderables.h"

SongSelectRenderables::SongSelectRenderables()
{
	SetTextures();
	SetInitialVisibility();
}

void SongSelectRenderables::OnSongChange(Resource<Song>* song)
{
	// std::vector<ChartRenderables> mChartRenderables;
}

void SongSelectRenderables::SetTextures()
{
	mStageText.SetFont("data/engine/fonts/Impact.ttf");
	mStageText.SetText("1st STAGE");
	mStageBG.mColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);

	mSongInfoBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mSongInfoTitle.SetFont("data/engine/fonts/Rubik.ttf");
	mSongInfoArtist.SetFont("data/engine/fonts/Rubik.ttf");

	mP1ScoreBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mP1ScoreTitle.SetText("P1 Score");
	mP1ScoreTitle.SetFont("data/engine/fonts/Rubik.ttf");
	mP1Score.SetFont("data/engine/fonts/Rubik.ttf");
	mP2ScoreBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mP2ScoreTitle.SetText("P1 Score");
	mP2ScoreTitle.SetFont("data/engine/fonts/Rubik.ttf");
	mP2Score.SetFont("data/engine/fonts/Rubik.ttf");

	mP1Selector.SetTexture("data/engine/texture/SongSelect/P1 Selector.png");
	mP2Selector.SetTexture("data/engine/texture/SongSelect/P2 Selector.png");

	mArrowUp.SetTexture("data/engine/texture/SongSelect/ArrowDown.png");
	mArrowUp.transform.rotation = 180.0f;
	mArrowDown.SetTexture("data/engine/texture/SongSelect/ArrowDown.png");
}

void SongSelectRenderables::SetInitialVisibility()
{
	//mArrowDown.transform.pos = glm::vec3(400.0f, 0.0f, 0.0f);
	mStageText.transform.pos = glm::vec3(-800.0f, 450.0f, 0.0f);
	mStageText.transform.scale = glm::vec3(1.25f);
}
