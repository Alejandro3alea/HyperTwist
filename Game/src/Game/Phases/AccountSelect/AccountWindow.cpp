#include "AccountWindow.h"
#include "Graphics/Color.h"
#include "Game/Account.h"
#include "Math/MathUtils.h"

AccountWindow::AccountWindow(const uint32_t playerID) : mPlayerID(playerID)
{
	SetTexture("textures/UIRoundedSquare.png");
	mColor = Color::FromInt(186, 250, 55);
	transform.pos = glm::vec3(playerID == ACCOUNTS_PLAYER_1 ? -950 : 950, 0.0f, 1.5f);
	transform.scale = glm::vec3(1000, 1200, 1);
}

void AccountWindow::Update(const float dt)
{
	switch (mWindowState)
	{
	case AccountWindowState::eOpening:
		OpenWindow(dt);
		break;
	case AccountWindowState::eClosing:
		CloseWindow(dt);
		break;
	case AccountWindowState::eClosed:
		mOnAccountWindowClosed.Broadcast(mPlayerID);
		break;
	}
}

void AccountWindow::OpenWindow(const float dt)
{
	if (mTransitionTimer >= 1.0f)
	{
		mWindowState = AccountWindowState::eOpen;
	}

	mTransitionTimer += mWindowTransitionSpeed * dt;
	UpdateWindow();
}

void AccountWindow::CloseWindow(const float dt)
{
	if (mTransitionTimer <= 0.0f)
	{
		mWindowState = AccountWindowState::eClosed;
	}

	mTransitionTimer -= mWindowTransitionSpeed * dt;
	UpdateWindow();
}

void AccountWindow::UpdateWindow()
{
	mTransitionTimer = Math::Clamp(mTransitionTimer, 0.0f, 1.0f);
	transform.scale.x = Math::EaseOut(0.0f, 500.0f, mTransitionTimer);
	transform.scale.y = Math::EaseIn(0.0f, 600.0f, mTransitionTimer);
}
