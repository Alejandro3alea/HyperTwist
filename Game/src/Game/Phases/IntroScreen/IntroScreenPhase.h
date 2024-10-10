#pragma once
#include "Game/Phases/Phase.h"

#include <array>

struct Renderable;

struct IntroScreenPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;

	void TriggerTransition();
	void OnTransitionEnd();

private:
	void UpdateLoginTransition(const float dt);

public:
	Renderable* mLogoRenderer;
	std::array<Renderable*, 3> mTransitionRenderers;

private:
	bool mIsTransitionTriggered = false;
	float mTransitionTimer = 0.0f;
};