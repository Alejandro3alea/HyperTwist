#pragma once
#include "Game/Phases/Phase.h"

#include <string>
#include <array>

struct Renderable;
struct AccountWindow;

struct AccountSelectPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;

	void TriggerTransition();
	void UpdateIntroTransition(const float dt);
	void OnTransitionEnd();

	void UpdateActivatedPlayersUniform();

	void CloseAllWindows();

private:
	void SetupAccountIfFree(const uint32_t playerID, const std::string& playerNfc);

	void UpdateAccountWindows(const float dt);
	void OnWindowClosed(const uint32_t playerID);

public:
	std::array<Renderable*, 3> mTransitionRenderers;

	std::array<AccountWindow*, 2> mAccountWindows;

private:
	float mTransitionTimer = 0.0f;
	bool mIsInTransition = true;
};