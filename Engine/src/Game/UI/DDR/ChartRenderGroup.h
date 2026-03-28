#pragma once
#include "Graphics/Renderables/HoldNoteBodyRenderer.h"
#include "Graphics/Renderables/LineList.h"
#include "Graphics/Renderables/MineRenderer.h"
#include "Graphics/Renderables/NoteRenderer.h"
#include "Graphics/Renderables/Renderable.h"

#include "Receptors.h"

#include <memory>

struct Chart;
struct RenderPass;

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