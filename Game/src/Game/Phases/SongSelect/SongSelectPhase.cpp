#include "SongSelectPhase.h"
#include "Graphics/GfxMgr.h"
#include "Input/InputMgr.h"

#include <filesystem>

void SongSelectPhase::OnEnter()
{
	//GfxMgr->SetBackgroundShader(ResourceMgr->Load<Shader>("data/shaders/SongSelectBG.shader")); 
	GfxMgr->SetBackgroundTexture(ResourceMgr->Load<Texture>("data/engine/texture/SongSelect/MainBG.png"));

	mRenderables = new SongSelectRenderables();
    LoadSongs();
    SetupFilters();
}

void SongSelectPhase::OnTick(const float dt)
{
    if (InputMgr->isKeyPressed(SDL_SCANCODE_UP))
    {
        mNodeIdx -= 3;
        OnUpdateIdx();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
    {
        mNodeIdx -= 1;
        OnUpdateIdx();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_DOWN))
    {
        mNodeIdx += 3;
        OnUpdateIdx();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
    {
        mNodeIdx += 1;
        OnUpdateIdx();
    }
}

void SongSelectPhase::OnExit()
{
}

SongSelectNode* SongSelectPhase::GetNodeByIdx(const uint32_t mSelectedIdx)
{
    for (uint32_t i = 0; i < mFilters.size(); i++)
    {
        if (i == mSelectedIdx)
            return mFilters[i].get();

        if (mFilters[i]->IsOpen())
        {
            SongSelectNode* foundNode = mFilters[i]->GetNodeByIdx(mSelectedIdx - i - 1);
            if (foundNode != nullptr)
                return foundNode;
        }
    }

    return nullptr;
}

void SongSelectPhase::LoadSongs()
{
    std::string rootDirectory = "./";  // Starting directory (current directory in this case)

    try {
        // Iterate recursively over the directory and its subdirectories
        for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDirectory)) {
            // Check if the current entry is a regular file and has the extension .smd
            if (entry.is_regular_file() && entry.path().extension() == ".smd") {
                std::cout << entry.path() << std::endl;  // Print the full path of the .smd file
                mSongs.push_back(ResourceMgr->Load<Song>(entry.path().string()));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void SongSelectPhase::SetupFilters()
{
    mFilters.push_back(std::make_shared<SongSelectTestFilter>(mSongs));
}

uint32_t SongSelectPhase::GetDisplayedNodesCount()
{
    uint32_t result = 0;

    for (auto filter : mFilters)
    {
        if (filter->IsOpen())
        {
            result += GetDisplayedNodesCount();
        }

        result++;
    }

    return result;
}

void SongSelectPhase::OnUpdateIdx()
{
}

void SongSelectPhase::OnSelectNode()
{
}
