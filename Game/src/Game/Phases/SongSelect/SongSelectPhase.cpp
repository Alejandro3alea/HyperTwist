#include "SongSelectPhase.h"
#include "Graphics/GfxMgr.h"
#include "Input/InputMgr.h"
#include "Math/Easing.h"

#include <filesystem>

std::vector<Resource<Song>*> SongSelectPhase::mSongs;

void SongSelectPhase::OnEnter()
{
	//GfxMgr->SetBackgroundShader(ResourceMgr->Load<Shader>("data/shaders/SongSelectBG.shader")); 
	GfxMgr->SetBackgroundTexture(ResourceMgr->Load<Texture>("data/engine/texture/SongSelect/MainBG.png"));

	mRenderables = std::make_shared<SongSelectRenderables>();
    ChangeToState(SongSelectState::FilterSelect);
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

void SongSelectPhase::ChangeToState(const SongSelectState& newState)
{
    mState = newState;

    switch (mState)
    {
    case SongSelectState::FilterSelect:
        TransitionToFilterSelect();
        break;
    case SongSelectState::SongSelect:
        TransitionToSongSelect();
        break;
    case SongSelectState::DifficultySelect:
        TransitionToDifficultySelect();
        break;
    }
}


void SongSelectPhase::TransitionToFilterSelect()
{
    mRenderables->mArrowDown.mbIsVisible = false;
    mRenderables->mArrowUp.mbIsVisible = false;
    mRenderables->mP1Score.mbIsVisible = false;
    mRenderables->mP1ScoreBG.mbIsVisible = false;
    mRenderables->mP1ScoreTitle.mbIsVisible = false;
    mRenderables->mP1Selector.mbIsVisible = false;
    mRenderables->mP2Score.mbIsVisible = false;
    mRenderables->mP2ScoreBG.mbIsVisible = false;
    mRenderables->mP2ScoreTitle.mbIsVisible = false;
    mRenderables->mP2Selector.mbIsVisible = false;

    mRenderables->mSongInfoArtist.mbIsVisible = false;
    mRenderables->mSongInfoBG.mbIsVisible = false;
    mRenderables->mSongInfoTitle.mbIsVisible = false;
    mRenderables->mSongThumb.mbIsVisible = false;

    for (auto& filter : mFilters)
        filter->mRenderable.mbIsVisible = true;
}

void SongSelectPhase::TransitionToSongSelect()
{
    mRenderables->mArrowDown.mbIsVisible = true;
    mRenderables->mArrowUp.mbIsVisible = true;
    mRenderables->mP1Score.mbIsVisible = true;
    mRenderables->mP1ScoreBG.mbIsVisible = true;
    mRenderables->mP1ScoreTitle.mbIsVisible = true;
    mRenderables->mP1Selector.mbIsVisible = true;
    mRenderables->mP2Score.mbIsVisible = true;
    mRenderables->mP2ScoreBG.mbIsVisible = true;
    mRenderables->mP2ScoreTitle.mbIsVisible = true;
    mRenderables->mP2Selector.mbIsVisible = true;

    mRenderables->mSongInfoArtist.mbIsVisible = true;
    mRenderables->mSongInfoBG.mbIsVisible = true;
    mRenderables->mSongInfoTitle.mbIsVisible = true;
    mRenderables->mSongThumb.mbIsVisible = true;

    for (auto& filter : mFilters)
        filter->mRenderable.mbIsVisible = false;
}

void SongSelectPhase::TransitionToDifficultySelect()
{
    mRenderables->mArrowDown.mbIsVisible = false;
    mRenderables->mArrowUp.mbIsVisible = false;
}


void SongSelectPhase::UpdateFilterSelect(const float dt)
{
    for (int32_t i = 0; i < mFilters.size(); i++)
    {
        int32_t diff = i - mFilterIdx;
        auto& t = mFilters[i]->mRenderable.transform;
        t.pos.x = Math::Lerp(t.pos.x, diff * 625.0f, 0.075f);
        t.pos.z = 10 - std::abs(diff);
        t.scale.x = Math::Lerp(t.scale.x, std::max(350.0f - 120.0f * std::abs(diff), 0.01f), 0.075f);
        t.scale.y = Math::Lerp(t.scale.y, std::max(350.0f - 120.0f * std::abs(diff), 0.01f), 0.075f);
    }

    if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
    {
        mFilterIdx = std::max(mFilterIdx - 1, 0);
        OnUpdateFilters();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
    {
        mFilterIdx = std::min(mFilterIdx + 1, static_cast<int32_t>(mFilters.size() - 1));
        OnUpdateFilters();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
    {
        mFilters[mFilterIdx]->OnOpen();
        OnSelectFilter();
        ChangeToState(SongSelectState::SongSelect);
    }
}

void SongSelectPhase::UpdateSongSelect(const float dt)
{
    if (InputMgr->isKeyPressed(SDL_SCANCODE_UP))
    {
        mNodeIdx -= 3;
        OnUpdateNodes();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
    {
        mNodeIdx -= 1;
        OnUpdateNodes();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_DOWN))
    {
        mNodeIdx += 3;
        OnUpdateNodes();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
    {
        mNodeIdx += 1;
        OnUpdateNodes();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
    {
        GetNodeByIdx(mNodeIdx)->OnOpen();
        ChangeToState(SongSelectState::DifficultySelect);
    }
}

void SongSelectPhase::UpdateDifficultySelect(const float dt)
{
}

#pragma region GetSongRegions
SongSelectNode* SongSelectPhase::GetNodeByIdx(const uint32_t mSelectedIdx)
{
    return mFilters[mFilterIdx]->GetNodeByIdx(mSelectedIdx);
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
#pragma endregion

void SongSelectPhase::LoadSongs()
{
    std::string rootDirectory = "data/songs/";

    for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDirectory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".smd") 
        {
            std::cout << entry.path() << std::endl;
            mSongs.push_back(ResourceMgr->Load<Song>(entry.path().string()));
        }
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

    for (int32_t i = 0; i < mFilters.size(); i++)
    {
        int32_t diff = i - mFilterIdx;
        auto& t = mFilters[i]->mRenderable.transform;
        t.pos.x = diff * 625.0f;
        t.pos.z = 10 - std::abs(diff);
        t.scale.x = std::max(350.0f - 120.0f * std::abs(diff), 0.01f);
        t.scale.y = std::max(350.0f - 120.0f * std::abs(diff), 0.01f);
    }
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

void SongSelectPhase::OnUpdateFilters()
{
}

void SongSelectPhase::OnUpdateNodes()
{
}

void SongSelectPhase::OnSelectFilter()
{
}

void SongSelectPhase::OnSelectNode()
{
}

void SongSelectPhase::OnCancellingNode()
{
}
