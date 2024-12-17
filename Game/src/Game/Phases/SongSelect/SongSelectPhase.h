#pragma once
#include "Game/Phases/Phase.h"
#include "SongSelectRenderables.h"
#include "SongSelectGroups.h"

struct SongSelectPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;

	SongSelectNode* GetNodeByIdx(const uint32_t mSelectedIdx);

protected:
	void LoadSongs();
	void SetupFilters();

	uint32_t GetDisplayedNodesCount();

	void OnUpdateIdx();
	void OnSelectNode();

private:
	uint32_t mNodeIdx;
	SongSelectRenderables* mRenderables;
	std::vector<std::shared_ptr<SongSelectFilter>> mFilters;
	std::vector<Resource<Song>*> mSongs;
};