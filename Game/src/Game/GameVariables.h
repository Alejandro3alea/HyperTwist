#pragma once
#include "Misc/DataTypes.h"
#include "Game/Song.h"
#include "Account.h"

struct GameVariables
{
    std::shared_ptr<Song> mSelectedSong;
    std::array<std::shared_ptr<Chart>, MAX_PLAYER_COUNT> mSelectedCharts;
};

static GameVariables gGameVariables;