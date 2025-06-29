#include "SongSelectPhase.h"
#include "Graphics/GfxMgr.h"
#include "Input/InputMgr.h"
#include "Math/Easing.h"
#include "Game/Phases/PhaseManager.h"
#include "Game/Account.h"
#include "Game/GameVariables.h"

#include <filesystem>

std::vector<Resource<Song>*> SongSelectPhase::mSongs;

void SongSelectPhase::OnEnter()
{
	//GfxMgr->SetBackgroundShader(ResourceMgr->Load<Shader>("shaders/SongSelectBG.shader")); 

	GfxMgr->SetBackgroundTexture(ResourceMgr->Load<Texture>("engine/texture/SongSelect/MainBG.png"));

	mSongSelectRenderables = std::make_shared<SongSelectRenderables>();
    mCommonRenderables = std::make_shared<SongSelectCommonRenderables>();

    ChangeToState(SongSelectState::FilterSelect);
    LoadSongs();
    SetupFilters();

    mSongDisplay.mOnSongFocus.Add([this](Song* song) { FocusSong(song); });
    mSongDisplay.mOnSongUnfocus.Add([this](Song* song) { UnfocusSong(song); });
    mSongDisplay.mOnSongSelect.Add([this](Song* song) { SelectSong(song); });

    mSelectorIndices[ACCOUNTS_PLAYER_1] = -1;
    mSelectorIndices[ACCOUNTS_PLAYER_2] = -1;
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
    case SongSelectState::EndTransition:
        TransitionToEndTransition();
        break;
    }
}

void SongSelectPhase::TransitionToFilterSelect()
{
    mSongSelectRenderables->mArrowDown.mbIsVisible = false;
    mSongSelectRenderables->mArrowUp.mbIsVisible = false;
    mSongSelectRenderables->Hide();
    
    for (auto& filter : mFilters)
        filter->mRenderable.mbIsVisible = true;

    mSongDisplay.Hide();
    mSongDisplay.ResetIndices();
    if (mCurrSong)
        UnfocusSong(mCurrSong);
}

void SongSelectPhase::TransitionToSongSelect()
{
    mSongSelectRenderables->mArrowDown.mbIsVisible = true;
    mSongSelectRenderables->mArrowUp.mbIsVisible = true;
    mSongSelectRenderables->Show();
    mDifficultySelectRenderables.reset();

    for (auto& filter : mFilters)
        filter->mRenderable.mbIsVisible = false;
}

void SongSelectPhase::TransitionToDifficultySelect()
{
    mCurrSong->LoadChartsSCD();
    mSongDisplay.Hide();
    mSongSelectRenderables->Hide();
    mDifficultySelectRenderables = std::make_shared<DifficultySelectRenderables>(mCurrSong, mSelectorIndices);
    mDifficultySelectRenderables->Show();
}

void SongSelectPhase::TransitionToEndTransition()
{
    auto getChart = [](i8 idx, const std::map<ChartDifficulty, Chart*>& charts) -> Chart* {
        for (const auto& [_diff, chart] : charts)
        {
            if (idx == 0)
                return chart;

            idx--;
        }
        return nullptr;
    };

    gGameVariables.mSelectedSong = std::shared_ptr<Song>(mCurrSong);
    for (u8 i = 0; i < mSelectorIndices.size(); i++)
        if (mSelectorIndices[i] < 0)
            gGameVariables.mSelectedCharts[i] = std::shared_ptr<Chart>(getChart(mSelectorIndices[i], mCurrSong->mCharts));

	PhaseMgr->ChangeToScene("Gameplay");
}

void SongSelectPhase::UpdateFilterSelect(const float dt)
{
    for (i32 i = 0; i < mFilters.size(); i++)
    {
        i32 diff = i - mFilterIdx;
        auto& t = mFilters[i]->mRenderable.transform;
        t.pos.x = Math::Lerp(t.pos.x, diff * 625.0f, 0.075f);
        t.pos.z = 10 - std::abs(diff);
        const float scaleVal = std::max(350.0f - 120.0f * std::abs(diff), 0.01f);
        t.scale.x = Math::Lerp(t.scale.x, scaleVal, 0.075f);
        t.scale.y = Math::Lerp(t.scale.y, scaleVal, 0.075f);
    }

    if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
    {
        mFilterIdx = std::max(mFilterIdx - 1, 0);
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
    {
        mFilterIdx = std::min(mFilterIdx + 1, static_cast<int32_t>(mFilters.size() - 1));
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
    {
        mFilters[mFilterIdx]->OnOpen();
        mSongDisplay.Reconstruct(mFilters[mFilterIdx].get());
        ChangeToState(SongSelectState::SongSelect);
    }
}

void SongSelectPhase::UpdateSongSelect(const float dt)
{
    mSongDisplay.UpdateDisplay();
    if (InputMgr->isKeyPressed(SDL_SCANCODE_UP))
    {
        mSongDisplay.MoveUp();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
    {
        mSongDisplay.MoveLeft();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_DOWN))
    {
        mSongDisplay.MoveDown();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
    {
        mSongDisplay.MoveRight();
    }
    
    if (InputMgr->isKeyPressed(SDL_SCANCODE_Z) && mCurrSong != nullptr)
        mSelectorIndices[ACCOUNTS_PLAYER_1]--;

    if (InputMgr->isKeyPressed(SDL_SCANCODE_X) && mCurrSong != nullptr)
        mSelectorIndices[ACCOUNTS_PLAYER_1]++;

    
    if (InputMgr->isKeyPressed(SDL_SCANCODE_N) && mCurrSong != nullptr)
        mSelectorIndices[ACCOUNTS_PLAYER_2]--;

    if (InputMgr->isKeyPressed(SDL_SCANCODE_M) && mCurrSong != nullptr)
        mSelectorIndices[ACCOUNTS_PLAYER_2]++;

    if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
    {
        mSongDisplay.Select();
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_ESCAPE))
    {
        ChangeToState(SongSelectState::FilterSelect);
    }
    

    if (mCurrSong != nullptr)
    {
        mSelectorIndices[ACCOUNTS_PLAYER_1] = std::clamp(mSelectorIndices[ACCOUNTS_PLAYER_1], static_cast<int8_t>(0), static_cast<int8_t>(mCurrSong->mChartDifficulties.size() - 1));
        mSelectorIndices[ACCOUNTS_PLAYER_2] = std::clamp(mSelectorIndices[ACCOUNTS_PLAYER_2], static_cast<int8_t>(0), static_cast<int8_t>(mCurrSong->mChartDifficulties.size() - 1));
    }
    mSongSelectRenderables->UpdateSelectorPositions(mSelectorIndices);
}

void SongSelectPhase::UpdateDifficultySelect(const float dt)
{
    if (InputMgr->isKeyPressed(SDL_SCANCODE_Z))
    {
        if (mSelectorIndices[ACCOUNTS_PLAYER_1] > 0)
            mSelectorIndices[ACCOUNTS_PLAYER_1]--;
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_X))
    {
        if (mSelectorIndices[ACCOUNTS_PLAYER_1] < mCurrSong->mChartDifficulties.size() - 1)
            mSelectorIndices[ACCOUNTS_PLAYER_1]++;
    }

    if (InputMgr->isKeyPressed(SDL_SCANCODE_N))
    {
        if (mSelectorIndices[ACCOUNTS_PLAYER_2] > 0)
            mSelectorIndices[ACCOUNTS_PLAYER_2]--;
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_M))
    {
        if (mSelectorIndices[ACCOUNTS_PLAYER_2] < mCurrSong->mChartDifficulties.size() - 1)
            mSelectorIndices[ACCOUNTS_PLAYER_2]++;
    }
    mDifficultySelectRenderables->UpdateChartPositions(mSelectorIndices);

    if (InputMgr->isKeyPressed(SDL_SCANCODE_RETURN))
    {
        ChangeToState(SongSelectState::EndTransition);
    }
    if (InputMgr->isKeyPressed(SDL_SCANCODE_ESCAPE))
    {
        ChangeToState(SongSelectState::SongSelect);
    }
}

#pragma region GetSongRegions
SongSelectNode* SongSelectPhase::GetNodeByIdx(const uint32_t mSelectedIdx) const
{
    return mFilters[mFilterIdx]->GetNodeByIdx(mSelectedIdx);
}

std::map<u8, std::vector<Song*>> SongSelectPhase::GetSongsByName()
{
    std::map<u8, std::vector<Song*>> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        result[song->mTitle.at(0)].push_back(song);
    }
    return result;
}

std::map<u8, std::vector<Song*>> SongSelectPhase::GetSongsByArtist()
{
    std::map<u8, std::vector<Song*>> result;
    for (Resource<Song>* songRes : mSongs)
    {
        Song* song = songRes->get();
        result[song->mArtist.at(0)].push_back(song);
    }
    return result;
}

std::map<u32, std::vector<Song*>> SongSelectPhase::GetSongsByLevel()
{
    std::map<u32, std::vector<Song*>> result;
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
        if (song->mDisplayBPM >= static_cast<float>(minBPM) && song->mDisplayBPM < static_cast<float>(maxBPM))
            result.push_back(song);
    }
    return result;
}
#pragma endregion

void SongSelectPhase::LoadSongs()
{
    std::string rootDirectory = FileUtils::JoinPath(DATA_PATH, "songs/");

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

    for (i32 i = 0; i < mFilters.size(); i++)
    {
        i32 diff = i - mFilterIdx;
        auto& t = mFilters[i]->mRenderable.transform;
        t.pos.x = static_cast<f32>(diff * 625);
        t.pos.z = static_cast<f32>(10 - std::abs(diff));
        t.scale.x = std::max(350.0f - 120.0f * static_cast<f32>(std::abs(diff)), 0.01f);
        t.scale.y = std::max(350.0f - 120.0f * static_cast<f32>(std::abs(diff)), 0.01f);
    }
}

u32 SongSelectPhase::GetDisplayedNodesInGroup(SongSelectGroup* group)
{
    u32 result = 0;
    for (auto& child : group->mChildren)
    {
        if (!child->IsLeaf())
        {
            auto* childGroup = dynamic_cast<SongSelectGroup*>(child.get());
            if (childGroup->IsOpen())
                result += GetDisplayedNodesInGroup(childGroup);
        }

        result++;
    }

    return result;
}

std::pair<u32, u32> SongSelectPhase::GetDisplayData(
    const std::vector<std::shared_ptr<SongSelectNode>>& groups,
    u32 nodeIdx)
{
    u32 groupsTillIdx = 0;
    u32 nodesTillIdx = 0;
    for (auto& groupNode : groups)
    {
        groupsTillIdx++;
        if (nodeIdx == 0)
            return std::make_pair(groupsTillIdx, nodesTillIdx);

        nodeIdx--;
        if (!groupNode->IsOpen())
            continue;

        auto group = dynamic_cast<SongSelectGroup*>(groupNode.get());
        nodesTillIdx = group->mChildren.size();
        if (nodesTillIdx > nodeIdx)
            return std::make_pair(groupsTillIdx, nodeIdx);

        nodeIdx -= nodesTillIdx;
    }
    return std::make_pair(groupsTillIdx, nodesTillIdx);
}

void SongSelectPhase::FocusSong(Song* song)
{
    mCurrSong = song;
    song->GetSong()->Play(song->mSampleStart);

    // Setup chart renderables
    mSongSelectRenderables->OnSongChange(song);
}

void SongSelectPhase::UnfocusSong(Song* song)
{
    mCurrSong = nullptr;
    song->GetSong()->Stop();

    // No song, empty chart
    mSongSelectRenderables->OnSongChange(nullptr);
}

void SongSelectPhase::SelectSong(Song* song)
{
    ChangeToState(SongSelectState::DifficultySelect);
}
