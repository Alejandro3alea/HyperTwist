#pragma once
#include "Framebuffer.h"
#include "Renderables/IRenderable.h"
#include "Resources/Resource.h"

struct RenderPass
{
    virtual ~RenderPass() = default;

    RenderPass(std::unique_ptr<Framebuffer>& fb, Resource<Shader>* shader = nullptr)
        : mShader(shader), mFramebuffer(std::move(fb))
    {
    }

    RenderPass(RenderPass&& other) : mFramebuffer(std::move(other.mFramebuffer)) {}
    RenderPass& operator=(RenderPass&& other) noexcept
    {
        if (this != &other)
            mFramebuffer = std::move(other.mFramebuffer);

        return *this;
    }
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    // -----------------------------------------------------------------------------------

    void AddRenderable(IRenderable* renderable) { mRenderables.push_back(renderable); }

    virtual void Execute();

    void BindTexture(const u8 idx = 0) { mFramebuffer->BindTexture(idx); }
    void UnbindTexture() { mFramebuffer->UnbindTexture(); }

  protected:
    virtual void PreRender();

  protected:
    Resource<Shader>* mShader = nullptr;
    std::unique_ptr<Framebuffer> mFramebuffer;
    std::vector<IRenderable*> mRenderables;
};