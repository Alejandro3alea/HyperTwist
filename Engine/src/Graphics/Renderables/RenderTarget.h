#pragma once
#include "IRenderable.h"

#include <GL/gl.h>

struct RenderTarget : public IRenderable
{
    RenderTarget(const GLuint targetTexture, RenderPass* renderPass = nullptr);
    virtual void Render(Shader* shader = nullptr) override;

  public:
    GLuint mTargetTexture;
};