#pragma once
#include "TextureAtlas.h"

#include <array>

#define MAX_NOTES 10000

struct Chart;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct MineRenderer : public TextureAtlas
{
    MineRenderer(Chart* inChart);
    MineRenderer(Chart* inChart, RenderPass* renderPass);

    virtual void Render(Shader* shader = nullptr) override final;

    void UpdateParams();
    void UpdateVBOs();

  private:
    void Initialize();

  private:
    Chart* mChart = nullptr;

    GLuint mVAO;
    GLuint mFloatVBO; // All float/vec variables stored in a VBO

    std::array<glm::vec2, MAX_NOTES * 3> mFloatParams;
    std::vector<float> mXPositions;
};
