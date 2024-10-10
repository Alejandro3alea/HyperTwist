#pragma once
#include "Misc/Singleton.h"

#include <string>
#include <memory>
#include <array>

#define ACCOUNTS_PLAYER_1 0
#define ACCOUNTS_PLAYER_2 1

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
	uint16_t mIconID;
};

class AccountManager
{
	Singleton(AccountManager);

	void LoadAccountIfFree(const uint32_t playerIdx, const std::string& nfcID);

	bool IsPlayerSlotFree(const uint32_t playerIdx);
	bool IsPlayerSlotOccupied(const uint32_t playerIdx);

	bool AreAllPlayerSlotsFree();
	bool AreAllPlayerSlotsOccupied();

	bool IsAnyPlayerSlotOccupied();

	void UnloadAccount(const uint32_t playerIdx);

public:
	std::array<std::shared_ptr<Account>, 2> mPlayers;
};

#define AccountMgr AccountManager::Instance()