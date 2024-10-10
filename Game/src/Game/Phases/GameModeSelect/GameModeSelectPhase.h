#pragma once
#include "Game/Phases/Phase.h"

struct GameModeSelectPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;
};
