#include "GameplayPhase.h"
#include "Audio/AudioMgr.h"
#include "Game/Account.h"
#include "Game/GameVariables.h"
#include "Graphics/GfxMgr.h"
#include "Utils/GameUtils.h"

#include <algorithm>
#include <vector>

void GameplayPhase::OnEnter()
{
    PrintDebug("Player 1: Playing [{}]: {}",
               GameUtils::ChartDifficultyToStr(gGameVariables.mSelectedCharts[PLAYER_1_IDX]->mDifficultyCategory),
               gGameVariables.mSelectedCharts[PLAYER_1_IDX]->mDifficultyVal);
    PrintDebug("Player 2: Playing [{}]: {}",
               GameUtils::ChartDifficultyToStr(gGameVariables.mSelectedCharts[PLAYER_2_IDX]->mDifficultyCategory),
               gGameVariables.mSelectedCharts[PLAYER_2_IDX]->mDifficultyVal);

    for (u8 i = 0; i < mNoteRenderers.size(); i++)
    {
        if (AccountMgr->IsPlayerSlotOccupied(i))
        {
            mNoteRenderers[i] = std::make_shared<NoteRenderer>(gGameVariables.mSelectedCharts[i]);
            mHoldRenderers[i] = std::make_shared<HoldNoteBodyRenderer>(gGameVariables.mSelectedCharts[i]);
            mMineRenderers[i] = std::make_shared<MineRenderer>(gGameVariables.mSelectedCharts[i]);

            mNoteRenderers[i]->mbIsVisible = true;
            mHoldRenderers[i]->mbIsVisible = true;
            mMineRenderers[i]->mbIsVisible = true;
        }
    }

    mSongInfo = &gGameVariables.mSelectedSong->mSongInfo;
    gGameVariables.mSelectedSong->GetSong()->get()->Stop();
    GfxMgr->SetBackgroundTexture(gGameVariables.mSelectedSong->GetBackground());
    GfxMgr->ShowBackground();
    mStartTransitionTimer = 1.0f;
    mBeatStartCount = 4;
}

void GameplayPhase::OnTick(const float dt)
{
    switch (mState)
    {
    [[likely]] case GameplayState::Gameplay:
        GameplayUpdate(dt);
        break;
    [[unlikely]] case GameplayState::StartTransition:
        StartTransitionUpdate(dt);
        break;
    [[unlikely]] case GameplayState::EndTransition:
        EndTransitionUpdate(dt);
        break;
    }
}

void GameplayPhase::OnExit() {}

void GameplayPhase::ChangeToState(const GameplayState& newState)
{
    mState = newState;

    switch (mState)
    {
    case GameplayState::Gameplay:
        TransitionToGameplay();
        break;
    case GameplayState::EndTransition:
        TransitionToEndTransition();
        break;
    [[unlikely]] case GameplayState::StartTransition:
        PrintError("ChangeToState set to GameplayState::StartTransition");
        break;
    }
}

void GameplayPhase::TransitionToGameplay() { SetupMeasureController(); }

void GameplayPhase::TransitionToEndTransition() {}

void GameplayPhase::StartTransitionUpdate(const float dt)
{
    mStartTransitionTimer -= dt;

    if (mStartTransitionTimer <= 0.0f)
        ChangeToState(GameplayState::Gameplay);
}

void GameplayPhase::GameplayUpdate(const float dt) { mCurrMeasure += dt; }

void GameplayPhase::EndTransitionUpdate(const float dt) {}

void GameplayPhase::SetupMeasureController()
{
    AudioMgr->StopTimerQueue();
    mBPMIncrement = 60.0f / mSongInfo->mBPMs[0];

    f32 positiveOffset = mSongInfo->mOffset;
    while (positiveOffset < 0)
        positiveOffset += mBPMIncrement;

    PrintDebug("Song offset {}", mSongInfo->mOffset);

    // @TODO: ADD MINIMUM NEGATIVE OFFSET (Test: Antagonism)
    // @TODO: ADD TESTS?
    constexpr u32 initialBeats = 4;
    for (u32 i = 0; i < initialBeats; i++)
        AudioMgr->QueueAudioAfter(*AudioMgr->GetNoteTick(), mBPMIncrement * i - mSongInfo->mOffset + positiveOffset);

    AudioMgr->QueueAudioAfter(*gGameVariables.mSelectedSong->GetSong(), mBPMIncrement * initialBeats + positiveOffset);

    // mBPMIncrement * i - mSongInfo->mOffset  >= 0
    //               v  v  v
    // i >= mSongInfo->mOffset / mBPMIncrement

    // @TODO: CHANGE MAGIC NUMBER TO SOME LOGIC
    for (u32 i = std::max(0.0f, std::ceil(mSongInfo->mOffset / mBPMIncrement)); i <= 400; i++)
        AudioMgr->QueueSoundAtTimestamp(*AudioMgr->GetBeatTick(), mBPMIncrement * i - mSongInfo->mOffset);

    const std::vector<float> noteTimestamps = gGameVariables.mSelectedSong->GetNoteTimestamps(
        gGameVariables.mSelectedCharts[PLAYER_1_IDX]->mDifficultyCategory);

    for (const float noteTimestamp : noteTimestamps)
        AudioMgr->QueueSoundAtTimestamp(*AudioMgr->GetNoteTick(), noteTimestamp);

    AudioMgr->ResumeTimerQueue();
}
