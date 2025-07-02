#include "GameplayPhase.h"
#include "Game/Account.h"
#include "Game/GameVariables.h"
#include "Graphics/GfxMgr.h"
#include "Audio/AudioMgr.h"

void GameplayPhase::OnEnter()
{
    for (u8 i = 0; i < mNoteRenderers.size(); i++)
    {
        if (AccountMgr->IsPlayerSlotOccupied(i))
        {
            mNoteRenderers[i] = std::make_shared<NoteRenderer>(gGameVariables.mSelectedCharts[i].get());
            mHoldRenderers[i] = std::make_shared<HoldNoteBodyRenderer>(gGameVariables.mSelectedCharts[i].get());
            mMineRenderers[i] = std::make_shared<MineRenderer>(gGameVariables.mSelectedCharts[i].get());

            mNoteRenderers[i]->mbIsVisible = true;
            mHoldRenderers[i]->mbIsVisible = true;
            mMineRenderers[i]->mbIsVisible = true;
        }
    }
    
    mSongInfo = &gGameVariables.mSelectedSong->mSongInfo;
    gGameVariables.mSelectedSong->GetSong()->get()->Stop();
	GfxMgr->SetBackgroundTexture(gGameVariables.mSelectedSong->GetBackground());
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

void GameplayPhase::OnExit()
{
}

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

void GameplayPhase::TransitionToGameplay()
{
    SetupMeasureController();
}

void GameplayPhase::TransitionToEndTransition()
{
}

void GameplayPhase::StartTransitionUpdate(const float dt)
{
    mStartTransitionTimer -= dt;

    if (mStartTransitionTimer <= 0.0f)
        ChangeToState(GameplayState::Gameplay);
}

void GameplayPhase::GameplayUpdate(const float dt)
{
    mCurrMeasure += dt;
    
    HandleStartingBeats();
}

void GameplayPhase::EndTransitionUpdate(const float dt)
{
}

void GameplayPhase::SetupMeasureController()
{
    mBPMIncrement = 60.0f / mSongInfo->mBPMs[0];
    mCurrMeasure = 0.0f;
    mNextMeasure = mBPMIncrement;

    for (u32 i = 1; i <= 400; i++)
        AudioMgr->QueueSound(AudioMgr->GetBeatTick()->get(), mBPMIncrement * i);
}

void GameplayPhase::HandleStartingBeats()
{
    if (mCurrMeasure >= mNextMeasure)
    {
        mNextMeasure += mBPMIncrement;

        mBeatStartCount--;
        if (mBeatStartCount == -1)
        {
            f32 positiveVal = mSongInfo->mOffset;
            while (positiveVal < 0)
                positiveVal += mBPMIncrement;
            gGameVariables.mSelectedSong->Play(0.0f);
            std::cout << mSongInfo->mOffset << std::endl;
        }
    }
}
