#include "TextureAtlas.h"
#include "Game/GlobalVariables.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Shader.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

TextureAtlas::TextureAtlas() : Renderable() { SetShader("engine/shaders/TextureAtlas.shader"); }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

TextureAtlas::TextureAtlas(RenderPass* renderPass) : Renderable(renderPass)
{
    SetShader("engine/shaders/TextureAtlas.shader");
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void TextureAtlas::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uTextureScale", mTextureScale);
    currShader->SetUniform("uTextureOffset", mTextureOffset);

    currShader->SetUniform("uColor", mColor);
    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);
    GLuint quadVAO = GfxMgr->GetQuad();
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

glm::vec2 TextureAtlas::SetTextureScale(glm::uvec2 sizeInPixels)
{
    if (mTexture != nullptr)
        mTextureScale = {static_cast<float>(sizeInPixels.x) / mTexture->get()->GetSize().x,
                         static_cast<float>(sizeInPixels.y) / mTexture->get()->GetSize().y};
    else
        mTextureScale = {1.0f, 1.0f};

    return mTextureScale;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

glm::vec2 TextureAtlas::SetTextureOffset(glm::uvec2 offsetInPixels)
{
    if (mTexture != nullptr)
        mTextureOffset = {static_cast<float>(offsetInPixels.x) / mTexture->get()->GetSize().x,
                          static_cast<float>(offsetInPixels.y) / mTexture->get()->GetSize().y};
    else
        mTextureOffset = {0.0f, 0.0f};

    return mTextureOffset;
}
