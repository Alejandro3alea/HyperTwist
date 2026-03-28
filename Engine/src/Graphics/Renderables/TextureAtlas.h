#pragma once
#include "Renderable.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

struct TextureAtlas : public Renderable
{
    TextureAtlas();
    TextureAtlas(RenderPass* renderPass);

    virtual void Render(Shader* shader = nullptr) override;

    glm::vec2 SetTextureScale(glm::uvec2 sizeInPixels);
    glm::vec2 SetTextureOffset(glm::uvec2 offsetInPixels);
    glm::vec2 SetTextureScale(const unsigned xPixels, unsigned yPixels)
    {
        return SetTextureScale(glm::uvec2(xPixels, yPixels));
    }
    glm::vec2 SetTextureOffset(const unsigned xPixels, unsigned yPixels)
    {
        return SetTextureOffset(glm::uvec2(xPixels, yPixels));
    }

  public:
    glm::vec2 mTextureScale;
    glm::vec2 mTextureOffset;
};