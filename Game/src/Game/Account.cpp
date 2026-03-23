#include "Account.h"

AccountManager* AccountManager::mpInstance;

void AccountManager::LoadAccountIfFree(const u32 playerIdx, const std::string& nfcID)
{
    if (IsPlayerSlotFree(playerIdx))
    {
        mPlayers[playerIdx] = std::make_shared<Account>(nfcID);
    }
}

void AccountManager::UnloadAccount(const u32 playerIdx) { mPlayers[playerIdx].reset(); }

bool AccountManager::IsPlayerSlotFree(const u32 playerIdx) { return mPlayers[playerIdx].get() == nullptr; }

bool AccountManager::IsPlayerSlotOccupied(const u32 playerIdx)
{
    return true;
    // return mPlayers[playerIdx].get() != nullptr;
}

std::array<bool, MAX_PLAYER_COUNT> AccountManager::GetOccupiedSlots()
{
    std::array<bool, MAX_PLAYER_COUNT> result;
    for (u8 i = 0; i < result.size(); i++)
        result[i] = mPlayers[i] != nullptr;
    return result;
}

bool AccountManager::AreAllPlayerSlotsFree()
{
    for (u32 i = 0; i < mPlayers.size(); i++)
    {
        if (IsPlayerSlotOccupied(i))
        {
            return false;
        }
    }

    return true;
}

bool AccountManager::AreAllPlayerSlotsOccupied()
{
    for (u32 i = 0; i < mPlayers.size(); i++)
    {
        if (IsPlayerSlotFree(i))
        {
            return false;
        }
    }

    return true;
}

bool AccountManager::IsAnyPlayerSlotOccupied() { return !AreAllPlayerSlotsFree(); }