#pragma once
#include "IRenderable.h"

#include <vector>

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct Chart;

struct LineList : public IRenderable
{
    LineList(Chart* inChart);
    LineList(Chart* inChart, RenderPass* renderPass);
    ~LineList();

    virtual void Render(Shader* shader = nullptr) override;

  private:
    void Initialize(Chart* inChart);

  private:
    std::vector<glm::vec2> ComputeLinePositions(Chart* inChart);

  private:
    Chart* mChart = nullptr;
    unsigned mVAO, mVBO;
    unsigned mLineCount = 0;
};
