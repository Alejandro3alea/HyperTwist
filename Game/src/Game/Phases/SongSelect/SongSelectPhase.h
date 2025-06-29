#pragma once
#include "Game/Phases/Phase.h"
#include "SongSelectCommonRenderables.h"
#include "SongSelectChartRenderables.h"
#include "DifficultySelectRenderables.h"
#include "SongSelectFilters.h"
#include "SongListDisplayManager.h"

#include <map>

enum class SongSelectState
{
	FilterSelect,
	SongSelect,
	DifficultySelect,
	EndTransition
};

struct SongSelectPhase final : public Phase
{
	void OnEnter() override;
	void OnTick(float dt) override;
	void OnExit() override;

	void ChangeToState(const SongSelectState& newState);

	SongSelectNode* GetNodeByIdx(u32 mSelectedIdx) const;

public:
	static std::map<u8, std::vector<Song*>> GetSongsByName();
	static std::map<u8, std::vector<Song*>> GetSongsByArtist();
	static std::map<u32, std::vector<Song*>> GetSongsByLevel();
	static std::map<std::string, std::vector<Song*>> GetSongsByPlatform();
	static std::vector<Song*> GetSongsByVersion(const std::vector<Song*>& platformSongs, const std::string& platformVersion);
	static std::vector<Song*> GetSongsFromBPMRange(u32 minBPM, u32 maxBPM);

protected:
	static void LoadSongs();
	void SetupFilters();

	u32 GetDisplayedNodesInGroup(SongSelectGroup* group);

	// return: <groups, song nodes>
	std::pair<u32, u32> GetDisplayData(
		const std::vector<std::shared_ptr<SongSelectNode>>& groups, u32 nodeIdx);

	void TransitionToFilterSelect();
	void TransitionToSongSelect();
	void TransitionToDifficultySelect();
	void TransitionToEndTransition();

	void UpdateFilterSelect(f32 dt);
	void UpdateSongSelect(f32 dt);
	void UpdateDifficultySelect(f32 dt);

	void FocusSong(Song* song);
	void UnfocusSong(Song* song);
	void SelectSong(Song* song);

private:
	Song* mCurrSong = nullptr;
	u32 mDisplayedRowCount = 3;
	i32 mFilterIdx = 0;
	f32 mTransitionVal = 0.0f;
	SongSelectState mState = SongSelectState::FilterSelect;
	std::vector<std::shared_ptr<SongSelectFilter>> mFilters;
	std::shared_ptr<SongSelectCommonRenderables> mCommonRenderables;
	std::shared_ptr<SongSelectRenderables> mSongSelectRenderables;
	std::shared_ptr<DifficultySelectRenderables> mDifficultySelectRenderables;
	std::array<i8, 2> mSelectorIndices = {};

	SongListDisplayManager mSongDisplay;

	static std::vector<Resource<Song>*> mSongs;
};