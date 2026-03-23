#pragma once
#include "Graphics/Renderable.h"

struct RenderPass;

struct Receptors
{
    Receptors(RenderPass* renderPass = nullptr);

    void Initialize();
    void Update(const glm::vec3& camPos);

    std::unique_ptr<TextureAtlas> mReceptorOn[4];
    std::unique_ptr<TextureAtlas> mReceptorOff[4];
};