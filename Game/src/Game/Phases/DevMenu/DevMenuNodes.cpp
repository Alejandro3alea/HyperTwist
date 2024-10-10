#include "DevMenuNodes.h"
#include "DevMenuActionLeaves.h"
#include "Graphics/GfxMgr.h"

#include "DevMenuPhase.h"
#include "Game/Phases/PhaseManager.h"

void DevMenuNode::InitializeRenderables()
{
    int yPos = 700;
    for (auto it : mItems)
    {
        std::shared_ptr<FontRenderer> currRenderable = std::make_shared<FontRenderer>(it->GetName());
        currRenderable->transform.pos.y = yPos;
        mRenderables.push_back(currRenderable);
        yPos -= 100;
    }
    UpdateRenderables();
    HideRenderables();
}

void DevMenuNode::ShowRenderables()
{
    for (auto it : mRenderables)
    {
        it->mbIsVisible = true;
    }

    UpdateRenderables();
}

void DevMenuNode::HideRenderables()
{
    for (auto it : mRenderables)
    {
        it->mbIsVisible = false;
    }
}

void DevMenuNode::AddItem(std::shared_ptr<DevMenuItem> item)
{
    mItems.push_back(item);
}

void DevMenuNode::OnSelected()
{
    DevMenuPhase* pDevPhase = dynamic_cast<DevMenuPhase*>(PhaseMgr->mCurrPhase.get());
    pDevPhase->ChangeCurrentNode(this);
}

void DevMenuNode::Select()
{
    if (mSelectedIdx >= 0 && mSelectedIdx < mItems.size())
    {
        mItems[mSelectedIdx]->OnSelected();
    }
}

void DevMenuNode::ResetSelectedIdx()
{
    mSelectedIdx = 0;
}

void DevMenuNode::IncrementSelectedIdx()
{
    mSelectedIdx++;
    UpdateSelectedIdx();
    UpdateRenderables();
}

void DevMenuNode::DecrementSelectedIdx()
{
    mSelectedIdx--;
    UpdateSelectedIdx();
    UpdateRenderables();
}

void DevMenuNode::UpdateSelectedIdx()
{
    if (mSelectedIdx < 0)
    {
        mSelectedIdx = mItems.size() - 1;
    }
    else if (mSelectedIdx >= mItems.size())
    {
        mSelectedIdx = 0;
    }
}

void DevMenuNode::UpdateRenderables()
{
    for (unsigned i = 0; i < mItems.size(); i++)
    {
        glm::vec4 textColor = (i == mSelectedIdx) ? glm::vec4(1.f, 0.f, 0.f, 1.f) : glm::vec4(1.0f);
        mRenderables[i]->mColor = textColor;
    }
}

DevMenuMainMenu::DevMenuMainMenu() : DevMenuNode("Main menu")
{
    AddItem(std::make_shared<DevMenuInputOutputCheck>(this));
    AddItem(std::make_shared<DevMenuScreenCheck>(this));
    AddItem(std::make_shared<DevMenuColorCheck>(this));
    AddItem(std::make_shared<DevMenuSoundSettings>(this));
    AddItem(std::make_shared<DevMenuGameSettings>(this));
    AddItem(std::make_shared<DevMenuCoinSettings>(this));
    AddItem(std::make_shared<DevMenuStartGame>());

	InitializeRenderables();
}

DevMenuInputOutputCheck::DevMenuInputOutputCheck(DevMenuNode* parentNode) : DevMenuNode("I/O check")
{
    AddItem(std::make_shared<DevMenuInputCheck>(this));
    AddItem(std::make_shared<DevMenuFootPanelCheck>(this));
    AddItem(std::make_shared<DevMenuICCardCheck>(this));
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}


DevMenuInputOutputCheck::DevMenuInputCheck::DevMenuInputCheck(DevMenuNode* parentNode) : DevMenuNode("Input check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuInputOutputCheck::DevMenuFootPanelCheck::DevMenuFootPanelCheck(DevMenuNode* parentNode) : DevMenuNode("Foot panel check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuInputOutputCheck::DevMenuICCardCheck::DevMenuICCardCheck(DevMenuNode* parentNode) : DevMenuNode("IC card check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}





DevMenuScreenCheck::DevMenuScreenCheck(DevMenuNode* parentNode) : DevMenuNode("Screen check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuColorCheck::DevMenuColorCheck(DevMenuNode* parentNode) : DevMenuNode("Color check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuSoundSettings::DevMenuSoundSettings(DevMenuNode* parentNode) : DevMenuNode("Sound settings")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuGameSettings::DevMenuGameSettings(DevMenuNode* parentNode) : DevMenuNode("Game settings")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuCoinSettings::DevMenuCoinSettings(DevMenuNode* parentNode) : DevMenuNode("Coin settings")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}