#include "GameplayRenderQueue.h"
#include "Game/Account.h"
#include "Graphics/GfxMgr.h"

void GameplayRenderQueue::Initialize()
{
    const glm::uvec2 hdSize = {1920, 1080};
    std::vector<std::shared_ptr<Buffer>> hdrBuf = {std::make_shared<HDRBuffer>(hdSize)};
    for (size_t player_idx = 0; player_idx < MAX_PLAYER_COUNT; player_idx++)
    {
        if (AccountMgr->IsPlayerSlotOccupied(player_idx))
        {
            auto fb = std::make_unique<Framebuffer>(hdrBuf);
            GenerateRenderTarget(fb->GetTexID(), player_idx);
            mPasses.push_back(RenderPass(fb));
        }
    }
}

void GameplayRenderQueue::Execute()
{
    IRenderQueue::Execute();

    GfxMgr->RenderScene();
}

void GameplayRenderQueue::GenerateRenderTarget(const u32 fbTexID, const u32 playerIdx)
{
    auto gameplayRenderTarget = std::make_unique<RenderTarget>(fbTexID);
    gameplayRenderTarget->mShader = ResourceMgr->Load<Shader>("engine/shaders/ChartRenderablesPass.shader");

    const glm::uvec2 projSize = {960, 540}; // { 1920, 1080 } / 2.0f

    const f32 relativePosX = playerIdx > PLAYER_1_IDX ? 0.55f : -0.55f;
    gameplayRenderTarget->transform.pos = {projSize.x * relativePosX, 0.0f, 1.0f + playerIdx};
    gameplayRenderTarget->transform.scale = {projSize.x, projSize.y, 1.0f};

    mGameplayRenderTargets.push_back(std::move(gameplayRenderTarget));
}
