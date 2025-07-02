#pragma once
#include "Game/Phases/Phase.h"
#include "Graphics/Renderable.h"
#include "Misc/DataTypes.h"
#include "Game/Account.h"
#include "Game/Song.h"

#include <memory>

struct Audio;

enum class GameplayState
{
	StartTransition,
	Gameplay,
	EndTransition
};

struct GameplayPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;

	void ChangeToState(const GameplayState& newState);

protected:
	void TransitionToGameplay();
	void TransitionToEndTransition();

	void StartTransitionUpdate(const float dt);
	void GameplayUpdate(const float dt);
	void EndTransitionUpdate(const float dt);

	void SetupMeasureController();

private:
	void HandleStartingBeats();
	
private:
	GameplayState mState;
	
	std::array<std::shared_ptr<NoteRenderer>, MAX_PLAYER_COUNT> mNoteRenderers;
	std::array<std::shared_ptr<HoldNoteBodyRenderer>, MAX_PLAYER_COUNT> mHoldRenderers;
	std::array<std::shared_ptr<MineRenderer>, MAX_PLAYER_COUNT> mMineRenderers;

	SongInfo* mSongInfo = nullptr;
	f32 mCurrMeasure = 0.0f;
	f32 mNextMeasure = 0.0f;
	f32 mBPMIncrement = 0.0f;

	f32 mStartTransitionTimer = 0.0f;
	i32 mBeatStartCount = 4;
};