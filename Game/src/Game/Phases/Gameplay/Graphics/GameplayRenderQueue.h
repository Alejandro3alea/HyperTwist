#pragma once
#include "Graphics/IRenderQueue.h"

struct GameplayRenderQueue : public IRenderQueue
{
    GameplayRenderQueue() = default;

    virtual void Initialize() override;
};