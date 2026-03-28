#include "FontRenderer.h"
#include "Graphics/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

FontRenderer::FontRenderer(const std::string& text, Resource<Font>* font)
{
    SetShader("engine/shaders/FontRenderer.shader");

    SetText(text);
    mFont = font;

    for (unsigned i = 0; i < FONTRENDERER_STRING_LIMIT; i++)
    {
        mTransforms[i] = glm::mat4(1.0f);
    }

    InitializeVertexData();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void FontRenderer::Render(Shader* shader)
{
    Font* pFont = mFont->get();
    if (pFont == nullptr)
    {
        return;
    }

    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    const Transform t = mParentTransform.has_value() ? transform + mParentTransform.value() : transform;
    currShader->SetUniform("uTextColor", glm::vec3(mColor.x, mColor.y, mColor.z));
    currShader->SetUniform("zLayer", t.pos.z);

    glActiveTexture(GL_TEXTURE0);
    mFont->get()->BindFontTexture();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glm::uvec2 textSize = GetTextSize();
    int workingIndex = 0;
    float x = t.pos.x - textSize.x / 2;
    float y = t.pos.y - textSize.y / 2;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = mText.begin(); c != mText.end(); c++)
    {
        Character ch = pFont->mCharacters[*c];

        if (*c == '\n')
        {
            y -= ((ch.Size.y)) * 1.3 * t.scale.y * FONTRENDERER_TEXTDEFAULTSIZE;
            x = t.pos.x;
        }
        else if (*c == ' ')
        {
            x += (ch.Advance >> 6) * t.scale.x * FONTRENDERER_TEXTDEFAULTSIZE;
        }
        else
        {
            float xpos = x + ch.Bearing.x * t.scale.x * FONTRENDERER_TEXTDEFAULTSIZE;
            float ypos = y - (256 - ch.Bearing.y) * t.scale.y * FONTRENDERER_TEXTDEFAULTSIZE;

            mTransforms[workingIndex] =
                glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, t.pos.z + workingIndex * 0.02f)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(256 * t.scale.x * FONTRENDERER_TEXTDEFAULTSIZE,
                                                      256 * t.scale.y * FONTRENDERER_TEXTDEFAULTSIZE, 0));
            mLetterMap[workingIndex] = ch.TextureID;

            // render quad
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * t.scale.x *
                 FONTRENDERER_TEXTDEFAULTSIZE; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of
                                               // 1/64th pixels by 64 to get amount of pixels))
            workingIndex++;
            if (workingIndex == FONTRENDERER_STRING_LIMIT)
            {
                TextRenderCall(workingIndex, currShader->mID);
                workingIndex = 0;
            }
        }
    }

    TextRenderCall(workingIndex, currShader->mID);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Resource<Font>* FontRenderer::SetFont(const std::string& path)
{
    mFont = ResourceMgr->Load<Font>(path);
    return mFont;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void FontRenderer::SetText(const std::string& text) { mText = text; }

void FontRenderer::InitializeVertexData()
{
    const GLfloat vertex_data[] = {
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FontRenderer::TextRenderCall(int length, GLuint shaderID)
{
    if (length != 0)
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uTransforms"), length, GL_FALSE, &mTransforms[0][0][0]);
        glUniform1iv(glGetUniformLocation(shaderID, "uLetterMap"), length, &mLetterMap[0]);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, length);
    }
}

glm::uvec2 FontRenderer::GetTextSize()
{
    glm::uvec2 totalSize = glm::uvec2(0);
    Font* pFont = mFont->get();
    if (pFont == nullptr)
    {
        return totalSize;
    }

    glm::uint currX = 0;

    const Transform t = mParentTransform.has_value() ? transform + mParentTransform.value() : transform;
    std::string::const_iterator c;
    for (c = mText.begin(); c != mText.end(); c++)
    {
        Character ch = pFont->mCharacters[*c];

        if (*c == '\n')
        {
            totalSize.y -= ((ch.Size.y)) * 1.3 * t.scale.y * FONTRENDERER_TEXTDEFAULTSIZE;
            totalSize.x = std::max(totalSize.x, currX);
            currX = t.pos.x;
        }
        else
        {
            currX += (ch.Advance >> 6) * t.scale.x * FONTRENDERER_TEXTDEFAULTSIZE;
        }
    }

    totalSize.x = std::max(totalSize.x, currX);
    return totalSize;
}
