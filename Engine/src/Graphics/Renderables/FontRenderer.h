#pragma once
#include "Graphics/Font.h"
#include "Graphics/Renderables/IRenderable.h"
#include "Resources/ResourceMgr.h"

#include <GL/gl.h>

#include <array>

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#define FONTRENDERER_TEXTDEFAULTSIZE 0.25f
#define FONTRENDERER_STRING_LIMIT 255
struct FontRenderer : public IRenderable
{
    FontRenderer(const std::string& text = "",
                 Resource<Font>* font = ResourceMgr->Load<Font>("engine/fonts/Roboto-Medium.ttf"));

    virtual void Render(Shader* shader = nullptr) override;
    Resource<Font>* SetFont(const std::string& path);
    void SetText(const std::string& text);

  private:
    void InitializeVertexData();

    void TextRenderCall(int length, GLuint shaderID);

    glm::uvec2 GetTextSize();

  private:
    Resource<Font>* mFont = nullptr;
    std::string mText;

    GLuint mVAO;
    GLuint mVBO;

    std::array<glm::mat4, FONTRENDERER_STRING_LIMIT> mTransforms;
    std::array<int, FONTRENDERER_STRING_LIMIT> mLetterMap;
};