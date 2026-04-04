#pragma once
#include "Graphics/IRenderQueue.h"
#include "Graphics/Renderables/RenderTarget.h"

struct GameplayRenderQueue : public IRenderQueue
{
    GameplayRenderQueue() { Initialize(); };

    virtual void Initialize() override final;

    virtual void Execute() override final;

  private:
    void GenerateRenderTarget(const u32 fbTexID, const u32 playerIdx);

  private:
    std::vector<std::unique_ptr<RenderTarget>> mGameplayRenderTargets;
};