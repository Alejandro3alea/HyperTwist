#include "GameplayRenderQueue.h"
#include "Account.h"

void GameplayRenderQueue::Initialize()
{
    constexpr glm::uvec2 hdSize = {1920, 1080};
    auto hdrBuf = {std::make_unique<HDRBuffer>(hdSize.x, hdSize.y)};
    auto fb = std::make_shared<Framebuffer>(hdrBuf);
    RenderPass pass(fb);
    for (size_t player_idx = 0; player_idx < MAX_PLAYER_COUNT; player_idx++)
    {
        if (AccountMgr->IsPlayerSlotOccupied(player_idx))
            mPasses.push_back(pass);
    }
}
