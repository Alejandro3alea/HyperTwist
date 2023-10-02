#include "Renderable.h"
#include "GfxMgr.h"

Renderable::Renderable()
{
    SetShader("data/engine/shaders/Quad.shader");
    SetTexture("data/engine/texture/White.png");

    GfxMgr->mRenderComps.push_back(this);
}

Renderable::~Renderable()
{
    std::remove(GfxMgr->mRenderComps.begin(), GfxMgr->mRenderComps.end(), this);
}

void Renderable::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uColor", mColor);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    GLuint quadVAO = GfxMgr->GetQuad();
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Resource<Shader>* Renderable::SetShader(const std::string& path)
{
    mShader = ResourceMgr->Load<Shader>(path);
    return mShader;
}

Resource<Texture>* Renderable::SetTexture(const std::string& path)
{
    mTexture = ResourceMgr->Load<Texture>(path);
    return mTexture;
}


TextureAtlas::TextureAtlas() : Renderable()
{
    SetShader("data/engine/shaders/TextureAtlas.shader");
}


void TextureAtlas::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uTextureScale", mTextureScale);
    currShader->SetUniform("uTextureOffset", mTextureOffset);

    currShader->SetUniform("uColor", mColor);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);
    GLuint quadVAO = GfxMgr->GetQuad();
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TextureAtlas::SetTextureScale(glm::uvec2 sizeInPixels)
{
    if (mTexture != nullptr)
        mTextureScale = { static_cast<float>(sizeInPixels.x) / mTexture->get()->GetSize().x, static_cast<float>(sizeInPixels.y) / mTexture->get()->GetSize().y };
    else
        mTextureScale = { 1.0f, 1.0f };
}

void TextureAtlas::SetTextureOffset(glm::uvec2 offsetInPixels)
{
    if (mTexture != nullptr)
        mTextureOffset = { static_cast<float>(offsetInPixels.x) / mTexture->get()->GetSize().x, static_cast<float>(offsetInPixels.y) / mTexture->get()->GetSize().y };
    else
        mTextureOffset = { 0.0f, 0.0f };
}

LineList::LineList(const std::vector<glm::vec2>& positions) : mLineCount(positions.size() / 2)
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    SetShader("data/engine/shaders/Line.shader");
}

LineList::~LineList()
{
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

void LineList::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uColor", mColor);

    glBindVertexArray(mVAO);

    // Draw the lines
    glDrawArrays(GL_LINES, 0, mLineCount);
}
