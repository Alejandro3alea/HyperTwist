#pragma once
#include "Game/Account.h"
#include "Game/Phases/Phase.h"
#include "Game/Song.h"
#include "Game/UI/DDR/ChartRenderGroup.h"
#include "Game/UI/DDR/Receptors.h"
#include "Graphics/GameplayRenderQueue.h"
#include "Graphics/PlayerGameplayUI.h"
#include "Graphics/Renderables/Renderable.h"
#include "Misc/DataTypes.h"

#include <memory>

struct Audio;

enum class GameplayState
{
    StartTransition,
    Gameplay,
    EndTransition
};

struct GameplayPhase : public Phase
{
    virtual void OnEnter() override;
    virtual void OnTick(const float dt) override;
    virtual void OnExit() override;

    void ChangeToState(const GameplayState& newState);

  protected:
    void TransitionToGameplay();
    void TransitionToEndTransition();

    void StartTransitionUpdate(const float dt);
    void GameplayUpdate(const float dt);
    void EndTransitionUpdate(const float dt);

    void SetupMeasureController();

  private:
    GameplayState mState;

    std::array<std::unique_ptr<ChartRenderGroup>, MAX_PLAYER_COUNT> mChartRenderers;
    std::array<std::unique_ptr<PlayerGameplayUI>, MAX_PLAYER_COUNT> mPlayerUIs;
    std::shared_ptr<GameplayRenderQueue> mRenderQueue;

    SongInfo* mSongInfo = nullptr;

    f32 mStartTransitionTimer = 0.0f;
    i32 mBeatStartCount = 4;
};