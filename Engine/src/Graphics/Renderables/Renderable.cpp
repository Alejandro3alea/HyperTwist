#include "Renderable.h"
#include "Game/GlobalVariables.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Shader.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Renderable::Renderable()
{
    SetShader("engine/shaders/Quad.shader");
    SetTexture("engine/texture/White.png");
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Renderable::Renderable(RenderPass* renderPass)
{
    SetShader("engine/shaders/Quad.shader");
    SetTexture("engine/texture/White.png");
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void Renderable::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uColor", mColor);
    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint quadVAO = GfxMgr->GetQuad();
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Resource<Texture>* Renderable::SetTexture(const std::string& path)
{
    mTexture = ResourceMgr->Load<Texture>(path);
    return mTexture;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Resource<Texture>* Renderable::SetTexture(Resource<Texture>* texture)
{
    mTexture = texture;
    return mTexture;
}

// @TODO: Debug OpenGL functions
/*
int getActiveVBOCount(GLuint vao)
{
    GLint maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    int activeVBOs = 0;

    for (int i = 0; i < maxVertexAttribs; ++i)
    {
        GLint bufferBinding;
        glBindVertexArray(vao);
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &bufferBinding);
        glBindVertexArray(0);

        if (bufferBinding != 0)
            activeVBOs++;
    }

    return activeVBOs;
}

bool isVertexAttributeEnabled(GLuint vao, GLuint attributeIndex)
{
    GLint isEnabled;
    glBindVertexArray(vao);
    glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &isEnabled);
    glBindVertexArray(0);

    return (isEnabled == GL_TRUE);
}
*/