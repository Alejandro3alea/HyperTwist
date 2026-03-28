#pragma once
#include "TextureAtlas.h"

#include <array>

#define MAX_NOTES 10000

struct Chart;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct HoldNoteBodyRenderer : public TextureAtlas
{
    HoldNoteBodyRenderer(Chart* inChart);
    HoldNoteBodyRenderer(Chart* inChart, RenderPass* renderPass);

    virtual void Render(Shader* shader = nullptr) override final;

    void UpdateParams();
    void UpdateVBOs();

  private:
    void Initialize();

  private:
    Chart* mChart = nullptr;

    GLuint mVAO;
    GLuint mFloatVBO; // All float/vec variables stored in a VBO

    std::array<float, MAX_NOTES * 4> mFloatParams;

    Resource<Texture>* mRollNoteTexture = nullptr;
    Resource<Texture>* mHoldBottomCapTexture = nullptr;
    Resource<Texture>* mRollBottomCapTexture = nullptr;
};