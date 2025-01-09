#pragma once
#include "Game/Phases/Phase.h"
#include "SongSelectRenderables.h"
#include "SongSelectFilters.h"

#include <map>

enum class SongSelectState
{
	FilterSelect,
	SongSelect,
	DifficultySelect
};

struct SongSelectPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;

	void ChangeToState(const SongSelectState& newState);
	void UpdateFilterSelect(const float dt);
	void UpdateSongSelect(const float dt);
	void UpdateDifficultySelect(const float dt);

	SongSelectNode* GetNodeByIdx(const uint32_t mSelectedIdx);

public:
	static std::map<uint8_t, std::vector<Song*>> GetSongsByName();
	static std::map<uint8_t, std::vector<Song*>> GetSongsByArtist();
	static std::map<uint32_t, std::vector<Song*>> GetSongsByLevel();
	static std::map<std::string, std::vector<Song*>> GetSongsByPlatform();
	static std::vector<Song*> GetSongsByVersion(const std::vector<Song*>& platformSongs, const std::string& platformVersion);
	static std::vector<Song*> GetSongsFromBPMRange(const uint32_t minBPM, const uint32_t maxBPM);

protected:
	static void LoadSongs();
	void SetupFilters();

	uint32_t GetDisplayedNodesCount();

	void OnUpdateFilterIdx();
	void OnUpdateNodeIdx();

	void OnSelectFilter();
	void OnSelectNode();
	void OnCancellingNode();

private:
	uint32_t mFilterIdx = 0;
	uint32_t mNodeIdx = 0;
	SongSelectState mState = SongSelectState::FilterSelect;
	std::vector<std::shared_ptr<SongSelectFilter>> mFilters;
	std::shared_ptr<SongSelectRenderables> mRenderables;

	static std::vector<Resource<Song>*> mSongs;
};