#include "SongSelectRenderables.h"
#include "Utils/GameUtils.h"

ChartRenderables::ChartRenderables(const ChartDifficulty& category, const uint8_t level)
{
	mBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");

	mDifficulty.SetFont("data/engine/fonts/Rubik.ttf");
	mDifficulty.SetText(GameUtils::ChartDifficultyToStr(category));

	mLevel.SetFont("data/engine/fonts/Rubik.ttf");
	mLevel.SetText(std::to_string(level));

	mP1Grade.SetTexture("data/engine/texture/Grades/AAA.png");
	mP2Grade.SetTexture("data/engine/texture/Grades/AAA.png");
}

SongSelectRenderables::SongSelectRenderables()
{
	SetTextures();
	SetInitialVisibility();
}

void SongSelectRenderables::OnSongChange(Song* song)
{
	mChartRenderables.clear();

	if (song == nullptr)
	{
		HideSelectedSongData();
		return;
	}

	UpdateSelectedSongData(song);

	// Setup chart renderables
	uint8_t i = 0;
	for (auto& [diffCategory, diffLevel] : song->mChartDifficulties)
	{
		mChartRenderables.push_back(CreateNewChartRenderables(diffCategory, diffLevel, i));
	}
}

ChartRenderables SongSelectRenderables::CreateNewChartRenderables(const ChartDifficulty& category, const uint8_t level, const uint8_t idx)
{
	ChartRenderables result(category, level);

	const float pos = 128.0f + idx * 75.0f;
	//result.mBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	//result.mBG.transform.pos = glm::vec3(-580.0f, 128.0f, 0.5f);
	//result.mBG.transform.scale = glm::vec3(260.0f, 40.0f, 0.1f);

	//result.mDifficulty.transform.pos = glm::vec3(-580.0f, 128.0f, 1.75f);
	result.mDifficulty.transform.scale = glm::vec3(10.7f);
	//result.mLevel.transform.pos = glm::vec3(-580.0f, 128.0f, 1.75f);
	//result.mLevel.transform.scale = glm::vec3(0.7f);

	//result.mP1Grade.transform.pos = glm::vec3(-650.0f, 128.0f, 1.75f);
	//result.mP1Grade.transform.scale = glm::vec3(50.0f);
	result.mP2Grade.transform.pos = glm::vec3(-510.0f, 128.0f, 1.75f);
	result.mP2Grade.transform.scale = glm::vec3(500.0f);

	return result;
}

void SongSelectRenderables::UpdateSelectedSongData(Song* song)
{
	ShowSelectedSongData();
	mSongInfoTitle.SetText(song->mTitle);
	mSongInfoArtist.SetText(song->mArtist);

	if (!song->mCDTitlePath.empty())
		mSongThumb.SetTexture(song->GetPath() + song->mCDTitlePath);
	else if (!song->mBannerPath.empty())
		mSongThumb.SetTexture(song->GetPath() + song->mBannerPath);
}

void SongSelectRenderables::ShowSelectedSongData()
{
	mSongInfoBG.mbIsVisible = true;
	mSongInfoTitle.mbIsVisible = true;
	mSongInfoArtist.mbIsVisible = true;
	mSongThumb.mbIsVisible = true;

	mP1ScoreBG.mbIsVisible = true;
	mP1ScoreTitle.mbIsVisible = true;
	mP1Score.mbIsVisible = true;
	mP2ScoreBG.mbIsVisible = true;
	mP2ScoreTitle.mbIsVisible = true;
	mP2Score.mbIsVisible = true;

	mP1Selector.mbIsVisible = true;
	mP2Selector.mbIsVisible = true;
}

void SongSelectRenderables::HideSelectedSongData()
{
	mSongInfoBG.mbIsVisible = false;
	mSongInfoTitle.mbIsVisible = false;
	mSongInfoArtist.mbIsVisible = false;
	mSongThumb.mbIsVisible = false;

	mP1ScoreBG.mbIsVisible = false;
	mP1ScoreTitle.mbIsVisible = false;
	mP1Score.mbIsVisible = false;
	mP2ScoreBG.mbIsVisible = false;
	mP2ScoreTitle.mbIsVisible = false;
	mP2Score.mbIsVisible = false;

	mP1Selector.mbIsVisible = false;
	mP2Selector.mbIsVisible = false;
}

void SongSelectRenderables::SetTextures()
{
	mStageText.SetFont("data/engine/fonts/Impact.ttf");
	mStageText.SetText("1st STAGE");

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

void SongSelectRenderables::SetInitialVisibility()
{
	// Creepily staring at these values (maybe @TODO for making a UI file IO system?)
	mStageText.transform.pos = glm::vec3(-820.0f, 465.0f, 3.0f);
	mStageText.transform.scale = glm::vec3(1.0f);
	mStageBG.transform.pos = glm::vec3(-825.0f, 490.0f, 2.5f);
	mStageBG.mColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);
	mStageBG.transform.scale = glm::vec3(150.0f, 50.0f, 0.1f);

	mSongThumb.transform.pos = glm::vec3(-580.0f, 245.0f, 0.5f);
	mSongThumb.transform.scale = glm::vec3(225.0f);
	mSongInfoBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	mSongInfoBG.transform.pos = glm::vec3(-580.0f, -32.5f, 0.5f);
	mSongInfoBG.transform.scale = glm::vec3(260.0f, 40.0f, 0.1f);
	mSongInfoTitle.transform.pos = glm::vec3(-580.0f, -35.0f, 1.75f);
	mSongInfoTitle.transform.scale = glm::vec3(0.7f);
	mSongInfoArtist.transform.pos = glm::vec3(-580.0f, -64.0f, 1.5f);
	mSongInfoArtist.transform.scale = glm::vec3(0.35f);

	mP1ScoreBG.transform.pos = glm::vec3(-160.0f, 420.5f, 0.5f);
	mP1ScoreBG.transform.scale = glm::vec3(140.0f, 40.0f, 0.1f);
	mP1ScoreBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	mP1ScoreTitle.transform.pos = glm::vec3(-160.0f, 437.5f, 1.0f);
	mP1ScoreTitle.transform.scale = glm::vec3(0.35f);
	mP1Score.transform.pos = glm::vec3(-160.0f, 400.5f, 1.0f);
	mP1Score.transform.scale = glm::vec3(0.5f);

	mP2ScoreBG.transform.pos = glm::vec3(-160.0f, 315.0f, 0.5f);
	mP2ScoreBG.transform.scale = glm::vec3(140.0f, 40.0f, 0.1f);
	mP2ScoreBG.mColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);
	mP2ScoreTitle.transform.pos = glm::vec3(-160.0f, 330.0f, 1.0f);
	mP2ScoreTitle.transform.scale = glm::vec3(0.35f);
	mP2Score.transform.pos = glm::vec3(-160.0f, 290.0f, 1.0f);
	mP2Score.transform.scale = glm::vec3(0.5f);

	mP1Selector.transform.pos = glm::vec3(-802.5f, -435.0f, 1.5f);
	mP1Selector.transform.scale = glm::vec3(40.0f);
	mP2Selector.transform.pos = glm::vec3(-332.5f, -295.0f, 1.5f);
	mP2Selector.transform.scale = glm::vec3(40.0f);

	mArrowDown.transform.pos = glm::vec3(890.0f, -490.0f, 0.01f);
	mArrowDown.transform.scale = glm::vec3(30.0f, 15.0f, 0.1f);
	mArrowUp.transform.pos = glm::vec3(890.0f, 490.0f, 0.01f);
	mArrowUp.transform.scale = glm::vec3(30.0f, 15.0f, 0.1f);
	mArrowUp.transform.rotation = 180.0f;
}