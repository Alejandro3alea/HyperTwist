#pragma once
#include "Graphics/IRenderQueue.h"

struct GameplayRenderQueue : public IRenderQueue
{
    GameplayRenderQueue() { Initialize(); };

    virtual void Initialize() override;

    virtual void Execute() override;
};