#include "GameplayPhase.h"
#include "Game/Account.h"
#include "Game/GameVariables.h"

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
}

void GameplayPhase::OnTick(const float dt)
{

}

void GameplayPhase::OnExit()
{
}

void GameplayPhase::ChangeToState(const GameplayState& newState)
{
}

void GameplayPhase::TransitionToGameplay()
{
}

void GameplayPhase::TransitionToEndTransition()
{
}

void GameplayPhase::StartTransitionUpdate(const float dt)
{
}

void GameplayPhase::GameplayUpdate(const float dt)
{
}

void GameplayPhase::EndTransitionUpdate(const float dt)
{
}