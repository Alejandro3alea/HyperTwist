#include "AccountSelectPhase.h"
#include "Graphics/Renderable.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Color.h"
#include "Math/MathUtils.h"
#include "Game/Account.h"
#include "Input/InputMgr.h"
#include "AccountWindow.h"

#include <glm/glm.hpp>

void AccountSelectPhase::OnEnter()
{
	GfxMgr->SetBackgroundShader(ResourceMgr->Load<Shader>("shaders/AccountSelectBG.shader"));

	mTransitionTimer = 0.0f;

	TriggerTransition();

	mIsInTransition = true;
}

void AccountSelectPhase::OnTick(const float dt)
{
	if (mIsInTransition)
	{
		UpdateIntroTransition(dt);
	}

	if (InputMgr->isKeyPressed(SDL_SCANCODE_KP_PLUS))
	{
		SetupAccountIfFree(ACCOUNTS_PLAYER_1, TEST_ACCOUNTS_NFC_PLAYER_1);
	}
	if (InputMgr->isKeyPressed(SDL_SCANCODE_KP_MINUS))
	{
		SetupAccountIfFree(ACCOUNTS_PLAYER_2, TEST_ACCOUNTS_NFC_PLAYER_2);
	}
	if (InputMgr->isKeyPressed(SDL_SCANCODE_KP_ENTER))
	{
		CloseAllWindows();
	}

	UpdateAccountWindows(dt);
}

void AccountSelectPhase::OnExit()
{
}

void AccountSelectPhase::TriggerTransition()
{
	std::array<glm::vec4, 3> transitionColors = {
		Color::FromInt(250, 252, 254),
		Color::FromInt(43, 237, 255),
		Color::FromInt(191, 242, 0)
	};

	std::array<glm::vec3, 3> transitionPosStart = {
		glm::vec3(200, 0, 2.0f),
		glm::vec3(100, 760, 2.01f),
		glm::vec3(0, -780, 2.02f),
	};

	for (unsigned i = 0; i < mTransitionRenderers.size(); i++)
	{
		mTransitionRenderers[i] = new Renderable();
		mTransitionRenderers[i]->SetTexture("textures/CircleLine.png");
		mTransitionRenderers[i]->mColor = transitionColors[i];
		mTransitionRenderers[i]->transform.pos = transitionPosStart[i];
		mTransitionRenderers[i]->transform.scale = glm::vec3(3000, 420 + i * 40, 1);
	}
}

void AccountSelectPhase::UpdateIntroTransition(const float dt)
{
	const float maxTransitionTime = 0.5f;
	mTransitionTimer += dt;
	if (mTransitionTimer >= maxTransitionTime)
	{
		OnTransitionEnd();
	}

	std::array<glm::vec3, 3> transitionPosStart = {
		glm::vec3(200, 0, 2.0f),
		glm::vec3(100, 760, 2.01f),
		glm::vec3(0, -780, 2.02f),
	};

	std::array<glm::vec3, 3> transitionPosEnd = {
		glm::vec3(5700, 0, 2.0f),
		glm::vec3(5000, 760, 2.01f),
		glm::vec3(6400, -780, 2.02f),
	};

	for (unsigned i = 0; i < mTransitionRenderers.size(); i++)
	{
		mTransitionRenderers[i]->transform.pos = Math::Lerp(transitionPosStart[i], transitionPosEnd[i], mTransitionTimer / maxTransitionTime);
	}
}

void AccountSelectPhase::OnTransitionEnd()
{
	for (unsigned i = 0; i < mTransitionRenderers.size(); i++)
	{
		mTransitionRenderers[i]->mbIsVisible = false;
	}

	for (unsigned i = 0; i < mAccountWindows.size(); i++)
	{
		if (AccountMgr->IsPlayerSlotOccupied(i))
		{
			mAccountWindows[i] = new AccountWindow(i);
			mAccountWindows[i]->mOnAccountWindowClosed.Add([this](const uint32_t playerID) { OnWindowClosed(playerID); });
		}
	}

	UpdateActivatedPlayersUniform();
	mIsInTransition = false;
}

void AccountSelectPhase::UpdateActivatedPlayersUniform()
{
	std::array<float, 2> activatedPlayersValues;
	for (unsigned i = 0; i < mAccountWindows.size(); i++)
	{
		activatedPlayersValues[i] = (mAccountWindows[i] != nullptr) ? 1.0f : 0.0f;
	}

	GfxMgr->GetBackgroundShader()->get()->Bind();
	GfxMgr->GetBackgroundShader()->get()->SetUniform("uActivatedPlayersValues", activatedPlayersValues);
}

void AccountSelectPhase::CloseAllWindows()
{
	for (unsigned i = 0; i < mAccountWindows.size(); i++)
	{
		if (mAccountWindows[i] != nullptr)
		{
			mAccountWindows[i]->mWindowState = AccountWindowState::eClosing;
		}
	}
}

void AccountSelectPhase::SetupAccountIfFree(const uint32_t playerID, const std::string& playerNfc)
{
	if (AccountMgr->IsPlayerSlotFree(playerID))
	{
		mAccountWindows[playerID] = new AccountWindow(playerID);
		mAccountWindows[playerID]->mOnAccountWindowClosed.Add([this](const uint32_t playerID) { OnWindowClosed(playerID); });
	}

	AccountMgr->LoadAccountIfFree(playerID, playerNfc);
	UpdateActivatedPlayersUniform();
}

void AccountSelectPhase::UpdateAccountWindows(const float dt)
{
	for (unsigned i = 0; i < mAccountWindows.size(); i++)
	{
		if (mAccountWindows[i] != nullptr)
		{
			mAccountWindows[i]->Update(dt);
		}
	}
}

void AccountSelectPhase::OnWindowClosed(const uint32_t playerID)
{
	delete mAccountWindows[playerID];
	mAccountWindows[playerID] = nullptr;
	UpdateActivatedPlayersUniform();
}
