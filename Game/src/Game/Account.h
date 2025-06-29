#pragma once
#include "Misc/Singleton.h"
#include "Misc/DataTypes.h"

#include <string>
#include <memory>
#include <array>

#define ACCOUNTS_PLAYER_1 0
#define ACCOUNTS_PLAYER_2 1
#define MAX_PLAYER_COUNT 2

#define ACCOUNTS_NFC_NULL "0000-0000-0000-0000"

#ifdef TEST_MODE
#define TEST_ACCOUNTS_NFC_PLAYER_1 "0000-0000-0000-0001"
#define TEST_ACCOUNTS_NFC_PLAYER_2 "0000-0000-0000-0002"
#endif

struct Account
{
	Account(const std::string& nfcID) : mNfcID(nfcID) {}

	std::string mNfcID;
	std::string mName;
	u16 mIconID;
};

class AccountManager
{
	Singleton(AccountManager);

	void LoadAccountIfFree(const u32 playerIdx, const std::string& nfcID);

	std::array<bool, MAX_PLAYER_COUNT> GetOccupiedSlots();
	bool IsPlayerSlotFree(const u32 playerIdx);
	bool IsPlayerSlotOccupied(const u32 playerIdx);

	bool AreAllPlayerSlotsFree();
	bool AreAllPlayerSlotsOccupied();

	bool IsAnyPlayerSlotOccupied();

	void UnloadAccount(const u32 playerIdx);

public:
	std::array<std::shared_ptr<Account>, MAX_PLAYER_COUNT> mPlayers;
};

#define AccountMgr AccountManager::Instance()