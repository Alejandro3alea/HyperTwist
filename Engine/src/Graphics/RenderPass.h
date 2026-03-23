#pragma once
#include "Framebuffer.h"
#include "Renderable.h"
#include "Resources/Resource.h"

struct RenderPass
{
    virtual ~RenderPass() = default;

    RenderPass(std::shared_ptr<Framebuffer>& fb, Resource<Shader>* shader = nullptr) : mShader(shader), mFramebuffer(fb)
    {
    }

    void AddRenderable(Renderable* renderable) { mRenderables.push_back(renderable); }

    virtual void Execute();

    void BindTexture(const u8 idx = 0) { mFramebuffer->BindTexture(idx); }
    void UnbindTexture() { mFramebuffer->UnbindTexture(); }

  protected:
    virtual void PreRender();

  protected:
    Resource<Shader>* mShader = nullptr;
    std::shared_ptr<Framebuffer> mFramebuffer;
    std::vector<Renderable*> mRenderables;
};