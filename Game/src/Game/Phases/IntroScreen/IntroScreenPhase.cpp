#include "IntroScreenPhase.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Color.h"
#include "Graphics/Renderable.h"
#include "Input/InputMgr.h"
#include "Game/Phases/PhaseManager.h"
#include "Math/MathUtils.h"
#include "Game/Account.h"
#include "Graphics/WindowMgr.h"

void IntroScreenPhase::OnEnter()
{
	GfxMgr->SetBackgroundTexture(ResourceMgr->Load<Texture>("textures/NextageLogo.png"));
	
	glm::uvec2 windowSize = WindowMgr->mCurrentWindow->Size();
	mIsTransitionTriggered = false;
	mTransitionTimer = 0.0f;
}

void IntroScreenPhase::OnTick(const float dt)
{
	// @TODO: Card scan
	if (InputMgr->isKeyPressed(SDL_SCANCODE_KP_PLUS))
	{
		std::string playerNfc = TEST_ACCOUNTS_NFC_PLAYER_1;
		AccountMgr->LoadAccountIfFree(ACCOUNTS_PLAYER_1, playerNfc);
	}
	if (InputMgr->isKeyPressed(SDL_SCANCODE_KP_MINUS))
	{
		std::string playerNfc = TEST_ACCOUNTS_NFC_PLAYER_2;
		AccountMgr->LoadAccountIfFree(ACCOUNTS_PLAYER_2, playerNfc);
	}

	if (mIsTransitionTriggered)
	{
		UpdateLoginTransition(dt);
	}
	else if (AccountMgr->IsAnyPlayerSlotOccupied())
	{
		TriggerTransition();
	}
}

void IntroScreenPhase::TriggerTransition()
{
	std::array<glm::vec4, 3> transitionColors = {
		Color::FromInt(250, 252, 254),
		Color::FromInt(43, 237, 255),
		Color::FromInt(191, 242, 0)
	};

	std::array<glm::vec3, 3> transitionPosStart = {
		glm::vec3(-5800, 0, 2.0f),
		glm::vec3(-5000, 760, 2.01f),
		glm::vec3(-6600, -780, 2.02f),
	};

	for (unsigned i = 0; i < mTransitionRenderers.size(); i++)
	{
		mTransitionRenderers[i] = new Renderable();
		mTransitionRenderers[i]->SetTexture("textures/CircleLine.png");
		mTransitionRenderers[i]->mColor = transitionColors[i];
		mTransitionRenderers[i]->transform.pos = transitionPosStart[i];
		mTransitionRenderers[i]->transform.scale = glm::vec3(3000, 420 + i * 40, 1);
	}

	mIsTransitionTriggered = true;
}

void IntroScreenPhase::OnTransitionEnd()
{
	PhaseMgr->ChangeToScene("AccountSelect");
}

void IntroScreenPhase::UpdateLoginTransition(const float dt)
{
	const float maxTransitionTime = 0.5f;
	mTransitionTimer += dt;
	if (mTransitionTimer >= maxTransitionTime)
	{
		OnTransitionEnd();
		return;
	}

	std::array<glm::vec3, 3> transitionPosStart = {
		glm::vec3(-5800, 0, 2.0f),
		glm::vec3(-5000, 760, 2.01f),
		glm::vec3(-6600, -780, 2.02f),
	};

	std::array<glm::vec3, 3> transitionPosEnd = {
		glm::vec3(200, 0, 2.0f),
		glm::vec3(100, 760, 2.01f),
		glm::vec3(0, -780, 2.02f),
	};

	for (unsigned i = 0; i < mTransitionRenderers.size(); i++)
	{
		mTransitionRenderers[i]->transform.pos = Math::Lerp(transitionPosStart[i], transitionPosEnd[i], mTransitionTimer / maxTransitionTime);
	}
}
