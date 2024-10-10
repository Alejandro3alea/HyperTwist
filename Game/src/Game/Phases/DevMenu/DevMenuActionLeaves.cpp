#include "DevMenuActionLeaves.h"
#include "Game/Phases/PhaseManager.h"
#include "DevMenuNodes.h"

#include "DevMenuPhase.h"
#include "Game/Phases/PhaseManager.h"

DevMenuActionLeaf::DevMenuActionLeaf(const std::string& name, std::function<void()> action) :
	DevMenuItem(name), mAction(action) {}


DevMenuStartGame::DevMenuStartGame() :
	DevMenuActionLeaf("Start Game", [this](){ StartGame(); }) {}


void DevMenuStartGame::StartGame()
{
	PhaseMgr->ChangeToScene("IntroScreen");
}

DevMenuReturnToParent::DevMenuReturnToParent(DevMenuNode* parentNode) :
	DevMenuActionLeaf("Return", [this]() { ReturnToParent(); }), mParentNode(parentNode) {}

void DevMenuReturnToParent::ReturnToParent()
{
	DevMenuPhase* pDevPhase = dynamic_cast<DevMenuPhase*>(PhaseMgr->mCurrPhase.get());
	pDevPhase->ChangeCurrentNode(mParentNode);
}
