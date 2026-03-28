#include "IRenderQueue.h"
#include "Misc/ColorConsole.h"

#include <algorithm>

void IRenderQueue::Execute()
{
    std::for_each(mPasses.begin(), mPasses.end(), [](RenderPass& pass) { pass.Execute(); });
}

RenderPass* IRenderQueue::GetPassFromIdx(const u8 idx)
{
    if (idx >= mPasses.size())
    {
        PrintError("RenderPass idx {} >= {} (mPasses.size)", idx, mPasses.size());
        return nullptr;
    }

    return &mPasses[idx];
}
