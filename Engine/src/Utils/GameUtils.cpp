#include "GameUtils.h"
#include "Game/Song.h"

void GameUtils::UpdateSCCSong(const std::string& path)
{
    Song* pSong = ResourceMgr->Load<Song>(path)->get();
}

void GameUtils::UpdateSMSong(const std::string& path)
{
    Song* pSong = ResourceMgr->Load<Song>(path)->get();
}
