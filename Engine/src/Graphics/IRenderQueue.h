#pragma once
#include "RenderPass.h"

struct IRenderQueue
{
    virtual ~IRenderQueue() = default;

    IRenderQueue() { Initialize(); }

    virtual void Initialize() = 0;

    virtual void Execute();

  protected:
    std::vector<RenderPass> mPasses;
};