#pragma once
#include "Misc/DataTypes.h"
#include "RenderPass.h"

struct IRenderQueue
{
    virtual ~IRenderQueue() = default;

    IRenderQueue() = default;

    virtual void Initialize() = 0;

    virtual void Execute();

    RenderPass* GetPassFromIdx(const u8 idx);

  protected:
    std::vector<RenderPass> mPasses;
};