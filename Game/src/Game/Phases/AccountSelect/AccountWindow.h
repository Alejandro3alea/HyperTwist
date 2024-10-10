#pragma once
#include "Graphics/Renderable.h"
#include "Composition/Events/Event.h"

CREATE_MULTICAST_EVENT(OnAccountWindowClosed, uint32_t);

enum class AccountWindowState
{
	eOpening,
	eOpen,
	eClosing,
	eClosed,
};

struct AccountWindow : public Renderable
{
	AccountWindow(const uint32_t playerID);

	void Update(const float dt);

private:
	void OpenWindow(const float dt);
	void CloseWindow(const float dt);
	void UpdateWindow();

public:
	const uint32_t mPlayerID;
	AccountWindowState mWindowState = AccountWindowState::eOpening;

	OnAccountWindowClosed mOnAccountWindowClosed;

private:
	float mTransitionTimer = 0.0f;

	const float mWindowTransitionSpeed = 3.5f;
};