#pragma once
#include "Game/Phases/Phase.h"
#include "SongSelectRenderables.h"

struct SongSelectPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;

private:
	SongSelectRenderables* mRenderables;
};