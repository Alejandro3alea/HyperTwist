#include "DifficultySelectRenderables.h"
#include "Game/Account.h"
#include "Math/Easing.h"
#include "Utils/GfxUtils.h"

#include <Utils/GameUtils.h>

DifficultySelectChartRenderables::DifficultySelectChartRenderables(const ChartDifficulty& category, uint8_t level,
                                                                   i8 chartIdx, u8 playerIdx)
    : mChartIdx(chartIdx), mPlayerIdx(playerIdx)
{
    mBG.SetTexture("engine/texture/SongSelect/RectangleHorizontalFade.png");
    mDifficulty.SetFont("engine/fonts/Glaser Stencil D Regular.ttf");
    mDifficulty.SetText("Challenge");
    mLevel.SetFont("engine/fonts/Glaser Stencil D Regular.ttf");
    mLevel.SetText("17");
    mGrade.SetTexture("engine/texture/Grades/AAA.png");
    mFullComboIcon.SetTexture("engine/texture/Grades/MarvelousFullCombo.png");
}

void DifficultySelectChartRenderables::Show()
{
    mBG.mbIsVisible = true;
    mDifficulty.mbIsVisible = true;
    mLevel.mbIsVisible = true;
    mGrade.mbIsVisible = true;
    mFullComboIcon.mbIsVisible = true;
}

void DifficultySelectChartRenderables::Hide()
{
    mBG.mbIsVisible = false;
    mDifficulty.mbIsVisible = false;
    mLevel.mbIsVisible = false;
    mGrade.mbIsVisible = false;
    mFullComboIcon.mbIsVisible = false;
}

Transform DifficultySelectChartRenderables::GetTransformByIndex(const std::array<i8, 2>& selectorIndices)
{
    const bool isPlayer2 = mPlayerIdx == PLAYER_2_IDX;
    const i8 dist = mChartIdx - selectorIndices[mPlayerIdx];
    auto sign = [](int d) { return d < 0 ? -1.0f : 1.0f; };

    // const float playerPos = isPlayer2 ? 1.0f : -1.0f;
    // const float xPos = (50.0f * std::abs(dist) + 275.0f) * playerPos;
    // float magnitude = std::pow(std::abs(dist), 0.7f);
    // const float yPos = sign(dist) * -275.0f * magnitude - 25.0f;
    // const float scaleVal = 1.0f - 0.24f * std::abs(dist);
    // const glm::vec3 pos = glm::vec3(xPos, yPos, -1.0f);
    // const glm::vec2 scale(scaleVal);

    const float playerPos = isPlayer2 ? 1.0f : -1.0f;
    const float xPos = (50.0f * std::abs(dist) + 275.0f) * playerPos;
    float magnitude = std::pow(std::abs(dist), 0.7f);
    const float yPos = sign(dist) * -275.0f * magnitude - 25.0f;
    const float scaleVal = 0.9f - 0.24f * std::abs(dist);
    const glm::vec3 pos = glm::vec3(xPos, yPos, -1.0f);
    const glm::vec2 scale(scaleVal);
    return Transform(pos, scale, 0.0f);
}

DifficultySelectRenderables::DifficultySelectRenderables(const Song* song, const std::array<i8, 2>& selectorIndices)
    : mTable("UI/DifficultySelect.json")
{
    SetTextures(song);
    SetPositions();
    SetChartRenderables(song, selectorIndices);
}

void DifficultySelectRenderables::UpdateChartPositions(const std::array<i8, 2>& selectorIndices)
{
    for (auto& chartRenderable : mChartRenderables)
    {
        auto& transformBG = chartRenderable->mBG.mParentTransform;
        auto& transformDifficulty = chartRenderable->mDifficulty.mParentTransform;
        auto& transformLevel = chartRenderable->mLevel.mParentTransform;
        auto& transformGrade = chartRenderable->mGrade.mParentTransform;
        auto& transformFullComboIcon = chartRenderable->mFullComboIcon.mParentTransform;

        const Transform newRenderableT =
            Math::Lerp(transformBG.value(), chartRenderable->GetTransformByIndex(selectorIndices), 0.2f);
        transformBG = newRenderableT;
        transformDifficulty = newRenderableT;
        transformLevel = newRenderableT;
        transformGrade = newRenderableT;
        transformFullComboIcon = newRenderableT;
    }
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

std::shared_ptr<DifficultySelectChartRenderables>
DifficultySelectRenderables::CreateNewChartRenderables(const ChartDifficulty& category, const u8 level,
                                                       const i8 chartIdx, const u8 playerIdx,
                                                       const std::array<i8, 2>& selectorIndices)
{
    std::shared_ptr<DifficultySelectChartRenderables> result =
        std::make_shared<DifficultySelectChartRenderables>(category, level, chartIdx, playerIdx);
    Transform globalTransform = result->GetTransformByIndex(selectorIndices);

    Renderable& chartRenderableBG = result->mBG;
    FontRenderer& chartRenderableDifficulty = result->mDifficulty;
    FontRenderer& chartRenderableLevel = result->mLevel;
    Renderable& chartRenderableGrade = result->mGrade;
    Renderable& chartRenderableFullComboIcon = result->mFullComboIcon;

    chartRenderableBG.mParentTransform = globalTransform;
    chartRenderableDifficulty.mParentTransform = globalTransform;
    chartRenderableLevel.mParentTransform = globalTransform;
    chartRenderableGrade.mParentTransform = globalTransform;
    chartRenderableFullComboIcon.mParentTransform = globalTransform;

    SET_TABLE_VALUES(mTable, chartRenderableBG);
    SET_TABLE_VALUES(mTable, chartRenderableDifficulty);
    SET_TABLE_VALUES(mTable, chartRenderableLevel);
    SET_TABLE_VALUES(mTable, chartRenderableGrade);
    SET_TABLE_VALUES(mTable, chartRenderableFullComboIcon);

    const std::array<glm::vec3, 6> bgColors = {GfxUtils::IntToRGB(0x3affd3), GfxUtils::IntToRGB(0xffce3a),
                                               GfxUtils::IntToRGB(0xff3a3a), GfxUtils::IntToRGB(0x3aff43),
                                               GfxUtils::IntToRGB(0xff3af3), GfxUtils::IntToRGB(0xff843a)};
    chartRenderableBG.mColor = glm::vec4(bgColors[category], 0.4f);
    chartRenderableDifficulty.SetText(GameUtils::ChartDifficultyToStr(category));
    chartRenderableLevel.SetText(std::to_string(level));

    return result;
}

void DifficultySelectRenderables::SetTextures(const Song* song)
{
    const SongInfo& info = song->mSongInfo;
    mSongThumb.SetTexture(
        ResourceMgr->LoadFromGlobalPath<Texture>(FileUtils::JoinPath(song->GetPath(), info.mCDTitlePath)));
    mSongInfoBG.SetTexture("engine/texture/SongSelect/RectangleHorizontalFadeOneSide.png");
    mSongInfoTitle.SetFont("engine/fonts/Rubik.ttf");
    mSongInfoTitle.SetText(info.mTitle);
    mSongInfoArtist.SetFont("engine/fonts/Rubik.ttf");
    mSongInfoArtist.SetText(info.mArtist);
    mSongInfoBPM.SetFont("engine/fonts/Rubik.ttf");
    mSongInfoBPM.SetText("BPM: " + GameUtils::GetBPMLabel(info.mBPMs));

    mP1ScoreBG.SetTexture("engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP1ScoreTitle.SetText("Score");
    mP1ScoreTitle.SetFont("engine/fonts/Rubik.ttf");
    mP1Score.SetFont("engine/fonts/Rubik.ttf");
    mP1Score.SetText("347.880");
    mP2ScoreBG.SetTexture("engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP2ScoreTitle.SetText("Score");
    mP2ScoreTitle.SetFont("engine/fonts/Rubik.ttf");
    mP2Score.SetFont("engine/fonts/Rubik.ttf");
    mP2Score.SetText("1.000.000");

    mP1StepArtistBG.SetTexture("engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP1StepArtistTitle.SetText("Step Artist");
    mP1StepArtistTitle.SetFont("engine/fonts/Rubik.ttf");
    mP1StepArtist.SetText("IAmChris4Life");
    mP1StepArtist.SetFont("engine/fonts/Rubik.ttf");

    mP2StepArtistBG.SetTexture("engine/texture/SongSelect/RectangleHorizontalFade.png");
    mP2StepArtistTitle.SetText("Step Artist");
    mP2StepArtistTitle.SetFont("engine/fonts/Rubik.ttf");
    mP2StepArtist.SetText("Balea");
    mP2StepArtist.SetFont("engine/fonts/Rubik.ttf");

    mArrowUp.SetTexture("engine/texture/SongSelect/ArrowDown.png");
    mArrowDown.SetTexture("engine/texture/SongSelect/ArrowDown.png");
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

    SET_TABLE_VALUES(mTable, mP1StepArtistBG);
    SET_TABLE_VALUES(mTable, mP1StepArtistTitle);
    SET_TABLE_VALUES(mTable, mP1StepArtist);

    SET_TABLE_VALUES(mTable, mP2StepArtistBG);
    SET_TABLE_VALUES(mTable, mP2StepArtistTitle);
    SET_TABLE_VALUES(mTable, mP2StepArtist);

    SET_TABLE_VALUES(mTable, mArrowDown);
    SET_TABLE_VALUES(mTable, mArrowUp);
    mArrowUp.transform.rotation = 180.0f;
}

void DifficultySelectRenderables::SetChartRenderables(const Song* song, const std::array<i8, 2>& selectorIndices)
{
    auto getChart = [](i8 idx, const std::map<ChartDifficulty, Chart*>& charts) -> Chart*
    {
        for (const auto& [_diff, chart] : charts)
        {
            if (idx == 0)
                return chart;

            idx--;
        }
        return nullptr;
    };
    // mP1StepArtist.SetText(getChart(selectorIndices[PLAYER_1_IDX], song->mCharts)->mStepArtist);
    // mP2StepArtist.SetText(getChart(selectorIndices[PLAYER_2_IDX], song->mCharts)->mStepArtist);

    auto generateChartsForPlayer = [&](u8 playerIdx)
    {
        if (selectorIndices[playerIdx] == -1)
            return;

        i8 chartIdx = 0;
        for (auto& chart : song->mCharts)
            mChartRenderables.push_back(CreateNewChartRenderables(chart.first, chart.second->mDifficultyVal, chartIdx++,
                                                                  playerIdx, selectorIndices));
    };
    generateChartsForPlayer(PLAYER_1_IDX);
    generateChartsForPlayer(PLAYER_2_IDX);
}
