#pragma once
#include "Graphics/Renderable.h"
#include "Receptors.h"

struct ChartRenderGroup
{
    ChartRenderGroup(Chart* inChart, RenderPass* renderPass = nullptr);

  public:
    std::shared_ptr<NoteRenderer> mNoteRenderer;
    std::shared_ptr<HoldNoteBodyRenderer> mHoldRenderer;
    std::shared_ptr<MineRenderer> mMineRenderer;
    std::shared_ptr<Receptors> mPlayerReceptor;
    std::unique_ptr<Renderable> mDarkBackground;
    std::unique_ptr<LineList> mMeasureLines;
};