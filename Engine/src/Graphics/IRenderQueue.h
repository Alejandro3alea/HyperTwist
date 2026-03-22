#pragma once
#include "RenderPass.h"

struct IRenderQueue
{
    virtual ~IRenderQueue() = default;

    virtual void Execute() = 0;

  protected:
    std::vector<RenderPass> mPasses;
};