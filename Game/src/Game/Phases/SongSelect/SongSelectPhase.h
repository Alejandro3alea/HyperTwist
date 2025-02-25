#pragma once
#include "Game/Phases/Phase.h"
#include "SongSelectRenderables.h"
#include "SongSelectFilters.h"
#include "SongListDisplayManager.h"

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

	uint32_t GetDisplayedNodesInGroup(SongSelectGroup* group);

	// return: <groups, song nodes>
	std::pair<uint32_t, uint32_t> GetDisplayData(
		const std::vector<std::shared_ptr<SongSelectNode>>& groups, uint32_t nodeIdx);

	void TransitionToFilterSelect();
	void TransitionToSongSelect();
	void TransitionToDifficultySelect();

	void UpdateFilterSelect(const float dt);
	void UpdateSongSelect(const float dt);
	void UpdateDifficultySelect(const float dt);

	void FocusSong(Song* song);
	void UnfocusSong(Song* song);
	void SelectSong(Song* song);

private:
	Song* mCurrSong = nullptr;
	uint32_t mDisplayedRowCount = 3;
	int32_t mFilterIdx = 0;
	float mTransitionVal = 0.0f;
	SongSelectState mState = SongSelectState::FilterSelect;
	std::vector<std::shared_ptr<SongSelectFilter>> mFilters;
	std::shared_ptr<SongSelectRenderables> mRenderables;
	std::array<int8_t, 2> mSelectorIndices;

	SongListDisplayManager mSongDisplay;

	static std::vector<Resource<Song>*> mSongs;
};