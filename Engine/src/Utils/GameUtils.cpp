#include "GameUtils.h"
#include "Game/Song.h"

void GameUtils::UpdateSSCSongToSMD(const std::string& path)
{
    Song* pSong = ResourceMgr->Load<Song>(path)->get();
    pSong->SaveToSMD();
    pSong->SaveToSCD();
}

void GameUtils::UpdateSMSongToSMD(const std::string& path)
{
    Song* pSong = ResourceMgr->Load<Song>(path)->get();
    pSong->SaveToSMD();
    //pSong->SaveToSCD();
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
