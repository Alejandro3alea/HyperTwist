#include "DevMenuNodes.h"
#include "DevMenuActionLeaves.h"
#include "Graphics/GfxMgr.h"
#include "Utils/GameUtils.h"

#include "DevMenuPhase.h"
#include "Game/Phases/PhaseManager.h"

#include <filesystem>
#include <fstream>

void DevMenuNode::InitializeRenderables(int32_t yPos = 330)
{
    for (auto it : mItems)
    {
        std::shared_ptr<FontRenderer> currRenderable = std::make_shared<FontRenderer>(it->GetName());
        currRenderable->transform.pos.y = yPos;
        currRenderable->transform.scale = glm::vec3(0.75f, 0.75f, 1.0f);
        mItemRenderables.push_back(currRenderable);
        yPos -= 75;
    }
    UpdateRenderables();
    HideRenderables();
}

void DevMenuNode::ShowRenderables()
{
    for (auto it : mItemRenderables)
        it->mbIsVisible = true;

    for (auto it : mCustomRenderables)
        it->mbIsVisible = true;

    UpdateRenderables();
}

void DevMenuNode::HideRenderables()
{
    for (auto it : mItemRenderables)
        it->mbIsVisible = false;

    for (auto it : mCustomRenderables)
        it->mbIsVisible = false;
}

void DevMenuNode::AddItem(const std::shared_ptr<DevMenuItem>& item)
{
    mItems.push_back(item);
}

void DevMenuNode::AddRenderable(const std::shared_ptr<Renderable>& renderable)
{
    mCustomRenderables.push_back(renderable);
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
        mItemRenderables[i]->mColor = textColor;
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
    AddItem(std::make_shared<InputCheck>(this));
    AddItem(std::make_shared<FootPanelCheck>(this));
    AddItem(std::make_shared<ICCardCheck>(this));
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}


DevMenuInputOutputCheck::InputCheck::InputCheck(DevMenuNode* parentNode) : DevMenuNode("Input check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuInputOutputCheck::FootPanelCheck::FootPanelCheck(DevMenuNode* parentNode) : DevMenuNode("Foot panel check")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuInputOutputCheck::ICCardCheck::ICCardCheck(DevMenuNode* parentNode) : DevMenuNode("IC card check")
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
    AddItem(std::make_shared<UpdateSongVersions>(this));
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}

DevMenuGameSettings::UpdateSongVersions::UpdateSongVersions(DevMenuNode* parentNode) : DevMenuNode("Update SM/SSC songs to SMD")
{
    const auto& renderable = std::make_shared<FontRenderer>("Songs updated.");
    renderable->mColor = glm::vec4(0.25f, 1.0f, 0.3f, 1.0f);
    AddRenderable(renderable);
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables(-450);
}

void DevMenuGameSettings::UpdateSongVersions::OnSelected()
{
    DevMenuNode::OnSelected();
    UpdateSongs();
}

void DevMenuGameSettings::UpdateSongVersions::UpdateSongs() const
{
    std::string rootDirectory = "data/songs/";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDirectory))
    {
        if (entry.is_regular_file() && (entry.path().extension() == ".sm" || entry.path().extension() == ".ssc"))
        {
            const std::string entryPath = entry.path().string();
            //if (!IsSongUpToDate(entryPath))
                UpdateSong(entryPath);
        }
    }
}

void DevMenuGameSettings::UpdateSongVersions::UpdateSong(const std::string& path) const
{
    GameUtils::UpdateSongToSMD(path);
}

bool DevMenuGameSettings::UpdateSongVersions::IsSongUpToDate(const std::string& path) const
{
    const std::string smdPath = ResourceMgr->GetPathWithoutExtension(path) + ".smd";
    const auto smdWriteTime = std::filesystem::last_write_time(smdPath);
    const auto oldWriteTime = std::filesystem::last_write_time(path);

    return oldWriteTime < smdWriteTime;
}



DevMenuCoinSettings::DevMenuCoinSettings(DevMenuNode* parentNode) : DevMenuNode("Coin settings")
{
    AddItem(std::make_shared<DevMenuReturnToParent>(parentNode));
    InitializeRenderables();
}