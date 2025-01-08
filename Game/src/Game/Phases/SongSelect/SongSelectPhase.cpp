#include "SongSelectPhase.h"
#include "Graphics/GfxMgr.h"
#include "Input/InputMgr.h"

#include <filesystem>

std::vector<Resource<Song>*> SongSelectPhase::mSongs;

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
    switch (mState)
    {
    case SongSelectState::FilterSelect:
        UpdateFilterSelect(dt);
        break;
    case SongSelectState::SongSelect:
        UpdateSongSelect(dt);
        break;
    case SongSelectState::DifficultySelect:
        UpdateDifficultySelect(dt);
        break;
    }
}

void SongSelectPhase::OnExit()
{
}

void SongSelectPhase::UpdateFilterSelect(const float dt)
{
}

void SongSelectPhase::UpdateSongSelect(const float dt)
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
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
    {
        GetNodeByIdx(mNodeIdx)->OnOpen();
    }
}

void SongSelectPhase::UpdateDifficultySelect(const float dt)
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

std::map<uint8_t, std::vector<Song*>> SongSelectPhase::GetSongsByName()
{
    std::map<uint8_t, std::vector<Song*>> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        result[song->mTitle.at(0)].push_back(song);
    }
    return result;
}

std::map<uint8_t, std::vector<Song*>> SongSelectPhase::GetSongsByArtist()
{
    std::map<uint8_t, std::vector<Song*>> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        result[song->mArtist.at(0)].push_back(song);
    }
    return result;
}

std::map<uint32_t, std::vector<Song*>> SongSelectPhase::GetSongsByLevel()
{
    std::map<uint32_t, std::vector<Song*>> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        for (const auto& [_diff, chart] : song->mCharts)
            result[chart->mDifficultyVal].push_back(song);
    }
    return result;
}

std::map<std::string, std::vector<Song*>> SongSelectPhase::GetSongsByPlatform()
{
    std::map<std::string, std::vector<Song*>> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        result[song->mPlatform].push_back(song);
    }
    return result;
}

std::vector<Song*> SongSelectPhase::GetSongsByVersion(const std::vector<Song*>& platformSongs, const std::string& platformVersion)
{
    std::vector<Song*> result;
    for (Song* song : platformSongs)
    {
        if (song->mPlatformVersion == platformVersion)
            result.push_back(song);
    }
    return result;
}

std::vector<Song*> SongSelectPhase::GetSongsFromBPMRange(const uint32_t minBPM, const uint32_t maxBPM)
{
    std::vector<Song*> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        if (song->mDisplayBPM >= minBPM && song->mDisplayBPM < maxBPM)
            result.push_back(song);
    }
    return result;
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
    mFilters.push_back(std::make_shared<SongSelectSortByName>());
    mFilters.push_back(std::make_shared<SongSelectSortByLevel>());
    mFilters.push_back(std::make_shared<SongSelectSortByArtist>());
    mFilters.push_back(std::make_shared<SongSelectSortByVersion>());
    mFilters.push_back(std::make_shared<SongSelectSortByBPM>());
    mFilters.push_back(std::make_shared<SongSelectSortByGenre>());
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
