#pragma once
#include "Composition/Transform.h"
#include "Graphics/Shader.h"
#include "Resources/Resource.h"

#include <optional>

struct RenderPass;

struct IRenderable
{
    virtual ~IRenderable();

    IRenderable();
    IRenderable(RenderPass* renderPass);

    virtual void Render(Shader* shader = nullptr) = 0;

    Resource<Shader>* SetShader(const std::string& path);
    Resource<Shader>* SetShader(Resource<Shader>* shader);

  public:
    Resource<Shader>* mShader = nullptr;
    bool mbIsVisible = true;

    std::optional<Transform> mParentTransform;
    Transform transform;
    glm::vec4 mColor = {1.0f, 1.0f, 1.0f, 1.0f};
};
