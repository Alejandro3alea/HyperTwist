#include "GameplayRenderQueue.h"
#include "Game/Account.h"

void GameplayRenderQueue::Initialize()
{
    const glm::uvec2 hdSize = {1920, 1080};
    std::vector<std::shared_ptr<Buffer>> hdrBuf = {std::make_shared<HDRBuffer>(hdSize)};
    auto fb = std::make_shared<Framebuffer>(hdrBuf);
    RenderPass pass(fb);
    for (size_t player_idx = 0; player_idx < MAX_PLAYER_COUNT; player_idx++)
    {
        if (AccountMgr->IsPlayerSlotOccupied(player_idx))
            mPasses.push_back(pass);
    }
}

void GameplayRenderQueue::Execute()
{
    IRenderQueue::Execute();

    for (size_t player_idx = 0, pass_idx = 0; player_idx < MAX_PLAYER_COUNT; player_idx++)
    {
        if (AccountMgr->IsPlayerSlotOccupied(player_idx))
        {
            glActiveTexture(GL_TEXTURE1);
            mPasses[pass_idx].BindTexture();
            // mShader->SetUniform("uTexture", 1);

            pass_idx++;
        }
    }
}
