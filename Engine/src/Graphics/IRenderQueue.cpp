#include "IRenderQueue.h"

void IRenderQueue::Execute()
{
    std::for_each(mPasses.begin(), mPasses.end(), [](RenderPass& pass) { pass.Execute(); });
}