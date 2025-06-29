#pragma once
#include "Game/Phases/Phase.h"
#include "Graphics/Renderable.h"

#include <memory>

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

	std::array<std::shared_ptr<NoteRenderer>, 2> mNoteRenderers;
	std::array<std::shared_ptr<HoldNoteBodyRenderer>, 2> mHoldRenderers;
	std::array<std::shared_ptr<MineRenderer>, 2> mMineRenderers;

};