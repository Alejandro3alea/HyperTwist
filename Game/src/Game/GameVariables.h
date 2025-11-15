#pragma once
#include "Account.h"
#include "Game/Song.h"
#include "Misc/DataTypes.h"

struct GameVariables
{
    Song* mSelectedSong;
    std::array<Chart*, MAX_PLAYER_COUNT> mSelectedCharts;
};

extern GameVariables gGameVariables;