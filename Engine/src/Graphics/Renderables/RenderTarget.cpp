#include "RenderTarget.h"
#include "Game/GlobalVariables.h"
#include "Graphics/GfxMgr.h"

RenderTarget::RenderTarget(const GLuint targetTexture) : IRenderable(), mTargetTexture(targetTexture)
{
    SetShader("engine/shaders/Quad.shader");
}

RenderTarget::RenderTarget(const GLuint targetTexture, RenderPass* renderPass)
    : IRenderable(renderPass), mTargetTexture(targetTexture)
{
    SetShader("engine/shaders/Quad.shader");
}

void RenderTarget::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uColor", mColor);
    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTargetTexture);
    currShader->SetUniform("uTexture", 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint quadVAO = GfxMgr->GetQuad();
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
