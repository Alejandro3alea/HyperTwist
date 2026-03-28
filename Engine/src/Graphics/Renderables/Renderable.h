#pragma once
#include "Graphics/Texture.h"

#include "IRenderable.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct RenderPass;

struct Renderable : public IRenderable
{
    Renderable();
    Renderable(RenderPass* renderPass);

    virtual void Render(Shader* shader = nullptr) override;

    Resource<Texture>* SetTexture(const std::string& path);
    Resource<Texture>* SetTexture(Resource<Texture>* texture);

  public:
    Resource<Texture>* mTexture = nullptr;
};