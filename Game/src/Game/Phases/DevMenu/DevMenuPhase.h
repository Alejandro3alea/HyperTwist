#pragma once
#include "Game/Phases/Phase.h"
#include "DevMenuNodes.h"

struct FontRenderer;
struct Renderable;

struct DevMenuPhase : public Phase
{
	virtual void OnEnter() override;
	virtual void OnTick(const float dt) override;
	virtual void OnExit() override;

	void ChangeCurrentNode(DevMenuNode* newNode);

public:
	FontRenderer* mTitleRenderer;
	Renderable* mLineRenderer;
	DevMenuNode* mCurrentNode;
};