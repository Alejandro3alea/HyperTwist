#include "SongSelectChartRenderables.h"
#include "Utils/GameUtils.h"
#include "Utils/StringUtils.h"
#include "Math/Easing.h"

SongSelectChartRenderables::SongSelectChartRenderables(const ChartDifficulty& category, const uint8_t level)
{
	mBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");

	mDifficulty.SetFont("data/engine/fonts/Rubik.ttf");
	mDifficulty.SetText(ToUpper(GameUtils::ChartDifficultyToStr(category)));

	mLevel.SetFont("data/engine/fonts/Rubik.ttf");
	mLevel.SetText(std::to_string(level));

	mP1Grade.SetTexture("data/engine/texture/Grades/AAA.png");
	mP2Grade.SetTexture("data/engine/texture/Grades/AAA.png");
}

void SongSelectChartRenderables::Show()
{
	mBG.mbIsVisible = true;
	mDifficulty.mbIsVisible = true;
	mLevel.mbIsVisible = true;
	mP1Grade.mbIsVisible = true;
	mP2Grade.mbIsVisible = true;
}

void SongSelectChartRenderables::Hide()
{
	mBG.mbIsVisible = false;
	mDifficulty.mbIsVisible = false;
	mLevel.mbIsVisible = false;
	mP1Grade.mbIsVisible = false;
	mP2Grade.mbIsVisible = false;
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
		Hide();
		return;
	}

	UpdateSelectedSongData(song);

	// Setup chart renderables
	uint8_t i = 0;
	for (auto& [diffCategory, diffLevel] : song->mChartDifficulties)
	{
		mChartRenderables.push_back(CreateNewChartRenderables(diffCategory, diffLevel, i));
		i++;
	}
}

void SongSelectRenderables::UpdateSelectorPositions(const std::array<int8_t, 2>& selectorIndices) 
{
	const float posP1 = GetYPosByIndex(selectorIndices[0]);
	const float posP2 = GetYPosByIndex(selectorIndices[1]);
	mP1Selector.transform.pos.y = Math::Lerp(mP1Selector.transform.pos.y, posP1, 0.2f);
	mP2Selector.transform.pos.y = Math::Lerp(mP2Selector.transform.pos.y, posP2, 0.2f);
}

std::shared_ptr<SongSelectChartRenderables> SongSelectRenderables::CreateNewChartRenderables(const ChartDifficulty& category, const uint8_t level, const uint8_t idx)
{
	std::shared_ptr<SongSelectChartRenderables> result = std::make_shared<SongSelectChartRenderables>(category, level);

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

	return result;
}

void SongSelectRenderables::UpdateSelectedSongData(Song* song)
{
	Show();
	mSongInfoTitle.SetText(song->mTitle);
	mSongInfoArtist.SetText(song->mArtist);

	if (!song->mCDTitlePath.empty())
		mSongThumb.SetTexture(song->GetPath() + song->mCDTitlePath);
	else if (!song->mBannerPath.empty())
		mSongThumb.SetTexture(song->GetPath() + song->mBannerPath);
}

void SongSelectRenderables::Show()
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

	for (auto it : mChartRenderables)
		it->Show();
}

void SongSelectRenderables::Hide()
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

	for (auto it : mChartRenderables)
		it->Hide();
}

void SongSelectRenderables::SetTextures()
{
	mSongInfoBG.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mSongInfoTitle.SetFont("data/engine/fonts/Rubik.ttf");
	mSongInfoTitle.SetText("Title");
	mSongInfoArtist.SetFont("data/engine/fonts/Rubik.ttf");
	mSongInfoArtist.SetText("Artist");

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

	mP1Selector.transform.pos = glm::vec3(-827.5f, -435.0f, 1.5f);
	mP1Selector.transform.scale = glm::vec3(40.0f);
	mP2Selector.transform.pos = glm::vec3(-332.5f, -295.0f, 1.5f);
	mP2Selector.transform.scale = glm::vec3(40.0f);

	mArrowDown.transform.pos = glm::vec3(890.0f, -490.0f, 0.01f);
	mArrowDown.transform.scale = glm::vec3(30.0f, 15.0f, 0.1f);
	mArrowUp.transform.pos = glm::vec3(890.0f, 490.0f, 0.01f);
	mArrowUp.transform.scale = glm::vec3(30.0f, 15.0f, 0.1f);
	mArrowUp.transform.rotation = 180.0f;
}

float SongSelectRenderables::GetYPosByIndex(const uint8_t idx)
{
	return static_cast<float>(-155 - idx * 70);
}
