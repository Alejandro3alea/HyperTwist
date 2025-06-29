#include "GameUtils.h"
#include "Game/Song.h"

#include <ranges>

void GameUtils::UpdateSongToSMD(const std::string& path)
{
    Song* pSong = ResourceMgr->Load<Song>(path)->get();
    pSong->SaveToSMD(path);
    pSong->SaveToSCD(path);
}

std::string GameUtils::ChartDifficultyToStr(const ChartDifficulty& val)
{
    switch (val)
    {
    case ChartDifficulty::Beginner:
        return "Beginner";
    case ChartDifficulty::Easy:
        return "Easy";
    case ChartDifficulty::Medium:
        return "Medium";
    case ChartDifficulty::Hard:
        return "Hard";
    case ChartDifficulty::Challenge:
        return "Challenge";
    case ChartDifficulty::Special:
        return "Special";
    default: // ChartDifficulty::None
        return "Unknown";
    }
}

ChartDifficulty GameUtils::StrToChartDifficulty(const std::string& str)
{
    if (str == "Beginner")
        return ChartDifficulty::Beginner;
    if (str == "Easy")
        return ChartDifficulty::Easy;
    if (str == "Medium")
        return ChartDifficulty::Medium;
    if (str == "Hard")
        return ChartDifficulty::Hard;
    if (str == "Challenge")
        return ChartDifficulty::Challenge;

    return ChartDifficulty::Special;
}

Grade GameUtils::ScoreToGrade(const i32 score, const bool fail, const bool fullCombo)
{
    if (fail || score < SCORE_D)
        return Grade::F;
    else if (score == SCORE_X && fullCombo)
        return Grade::X;
    else if (score >= SCORE_S && fullCombo)
        return Grade::S;
    else if (score >= SCORE_AAA)
        return Grade::AAA;
    else if (score >= SCORE_AA)
        return Grade::AA;
    else if (score >= SCORE_A)
        return Grade::A;
    else if (score >= SCORE_B)
        return Grade::B;
    else if (score >= SCORE_C)
        return Grade::C;
    else if (score >= SCORE_D)
        return Grade::D;

    return Grade::Unknown;
}

std::string GameUtils::GradeToStr(const Grade &grade)
{
    switch (grade)
    {
    case Grade::F:
        return "F";
    case Grade::D:
        return "D";
    case Grade::C:
        return "C";
    case Grade::B:
        return "B";
    case Grade::A:
        return "A";
    case Grade::AA:
        return "AA";
    case Grade::AAA:
        return "AAA";
    case Grade::S:
        return "S";
    case Grade::X:
        return "X";
    default:
        PrintWarning("Unknown grade sent to GradeToStr.");
        return "unknown";
    }
}

Resource<Texture> *GameUtils::GradeToTexture(const Grade &grade)
{
    return ResourceMgr->Load<Texture>("engine/texture/Grades/" + GradeToStr(grade) + ".png");
}

std::string GameUtils::GetBPMLabel(const std::map<float, float>& bpms)
{
    if (bpms.size() == 1)
        return std::to_string(static_cast<int32_t>(bpms.begin()->second));

    float smallest = std::numeric_limits<float>::max();
    float biggest = std::numeric_limits<float>::min();
    for (const float& currBPM : bpms | std::views::values)
    {
        if (currBPM < smallest)
            smallest = currBPM;

        if (currBPM > biggest)
            biggest = currBPM;
    }

    return std::to_string(static_cast<int32_t>(smallest)) + " - " + std::to_string(static_cast<int32_t>(biggest));
}