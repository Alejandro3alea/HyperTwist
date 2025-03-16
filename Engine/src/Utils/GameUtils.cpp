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

std::string GameUtils::GetBPMLabel(const std::map<float, float>& bpms)
{
    if (bpms.size() == 1)
        return std::to_string(bpms.begin()->second);

    float smallest = std::numeric_limits<float>::max();
    float biggest = std::numeric_limits<float>::min();
    for (const float& currBPM : bpms | std::views::values)
    {
        if (currBPM < smallest)
            smallest = currBPM;

        if (currBPM > biggest)
            biggest = currBPM;
    }

    return std::to_string(smallest) + " - " + std::to_string(biggest);
}