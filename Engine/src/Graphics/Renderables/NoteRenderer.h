#pragma once
#include "TextureAtlas.h"

#include <array>

#define MAX_NOTES 10000

struct Chart;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct NoteRenderer : public TextureAtlas
{
    NoteRenderer(Chart* inChart);
    NoteRenderer(Chart* inChart, RenderPass* renderPass);

    virtual void Render(Shader* shader = nullptr) override;

    void UpdateParams();
    void UpdateVBOs();

  private:
    void Initialize();

  protected:
    Chart* mChart = nullptr;

    GLuint mVAO;
    GLuint mFloatVBO; // All float/vec variables stored in a VBO

    std::array<glm::vec2, MAX_NOTES * 3> mFloatParams;

    std::vector<glm::mat4> mRotations;
    std::vector<float> mXPositions;
};
