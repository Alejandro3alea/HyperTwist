#include "DevMenuPhase.h"
#include "Input/InputMgr.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Renderable.h"
#include "Game/Phases/PhaseManager.h"

void DevMenuPhase::OnEnter()
{
	GfxMgr->SetBackgroundTexture(ResourceMgr->Load<Texture>("engine/texture/Black.png"));

	mTitleRenderer = new FontRenderer("Dev Menu", ResourceMgr->Load<Font>("engine/fonts/Roboto-Medium.ttf"));
	mTitleRenderer->transform.pos.y = 450.0f;
	mLineRenderer = new Renderable();
	mLineRenderer->mTexture = ResourceMgr->Load<Texture>("engine/texture/GradientLine.png");
	mLineRenderer->transform.pos.y = 425.0f;
	mLineRenderer->transform.pos.z = 10.0f;
	mLineRenderer->transform.scale = glm::vec3(750, 1, 1);

	ChangeCurrentNode(new DevMenuMainMenu);
}

void DevMenuPhase::OnTick(const float dt)
{
	if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
	{
		mCurrentNode->DecrementSelectedIdx();
	}
	else if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
	{
		mCurrentNode->IncrementSelectedIdx();
	}
	else if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
	{
		mCurrentNode->Select();
	}
}

void DevMenuPhase::OnExit()
{
}

void DevMenuPhase::ChangeCurrentNode(DevMenuNode* newNode)
{
	if (mCurrentNode)
	{
		mCurrentNode->HideRenderables();
	}
	mCurrentNode = newNode;
	mCurrentNode->ResetSelectedIdx();
	mTitleRenderer->SetText(mCurrentNode->GetName());
	mCurrentNode->ShowRenderables();
}
