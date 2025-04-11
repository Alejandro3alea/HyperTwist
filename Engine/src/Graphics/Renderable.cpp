#include "Renderable.h"
#include "GfxMgr.h"
#include "Game/Note.h"
#include "Game/Chart.h"
#include "Game/GameVariables.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <set>

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

Renderable::Renderable()
{
    SetShader("engine/shaders/Quad.shader");
    SetTexture("engine/texture/White.png");

    GfxMgr->mRenderComps.push_back(this);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

Renderable::~Renderable()
{
    auto& rendercomps = GfxMgr->mRenderComps;
    int renderCompsB = GfxMgr->mRenderComps.size();
    GfxMgr->mRenderComps.erase(
        std::remove(GfxMgr->mRenderComps.begin(), GfxMgr->mRenderComps.end(), this),
        GfxMgr->mRenderComps.end()
    );
    std::remove(GfxMgr->mRenderComps.begin(), GfxMgr->mRenderComps.end(), this);
    int renderCompsA = GfxMgr->mRenderComps.size();
    bool isDiff = renderCompsA != renderCompsB;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void Renderable::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uColor", mColor);
    currShader->SetUniform("uZoom", gGameVariables.mZoom);

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

Resource<Shader>* Renderable::SetShader(const std::string& path)
{
    mShader = ResourceMgr->Load<Shader>(path);
    return mShader;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

Resource<Shader>* Renderable::SetShader(Resource<Shader>* shader)
{
    mShader = shader;
    return mShader;
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

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

TextureAtlas::TextureAtlas() : Renderable()
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
    currShader->SetUniform("uZoom", gGameVariables.mZoom);

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
        mTextureScale = { static_cast<float>(sizeInPixels.x) / mTexture->get()->GetSize().x, static_cast<float>(sizeInPixels.y) / mTexture->get()->GetSize().y };
    else
        mTextureScale = { 1.0f, 1.0f };

    return mTextureScale;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

glm::vec2 TextureAtlas::SetTextureOffset(glm::uvec2 offsetInPixels)
{
    if (mTexture != nullptr)
        mTextureOffset = { static_cast<float>(offsetInPixels.x) / mTexture->get()->GetSize().x, static_cast<float>(offsetInPixels.y) / mTexture->get()->GetSize().y };
    else
        mTextureOffset = { 0.0f, 0.0f };

    return mTextureOffset;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

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

    SetShader("engine/shaders/Line.shader");
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

LineList::~LineList()
{
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void LineList::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uColor", mColor);
    currShader->SetUniform("uZoom", gGameVariables.mZoom);

    glBindVertexArray(mVAO);

    // Draw the lines
    glDrawArrays(GL_LINES, 0, mLineCount);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////


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
        glBindVertexArray(0); // Unbind the VAO

        if (bufferBinding != 0) 
            activeVBOs++;
    }

    return activeVBOs;
}

// Function to check if a vertex attribute is enabled
bool isAttributeEnabled(GLuint vao, GLuint attributeIndex) {
    GLint isEnabled;
    glBindVertexArray(vao);
    glGetVertexAttribiv(attributeIndex, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &isEnabled);
    glBindVertexArray(0);  // Unbind the VAO

    return (isEnabled == GL_TRUE);
}


NoteRenderer::NoteRenderer(Chart* inChart) : mChart(inChart)
{
    SetShader("engine/shaders/NoteRenderer.shader");
    mTexture = ResourceMgr->Load<Texture>("noteskins/USWCelSM5/_Down Tap Note 16x8.png");

    UpdateParams();

    mVAO = GfxMgr->CreateQuadModel();
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mFloatVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_NOTES * 3 * sizeof(glm::vec2), &mFloatParams[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)(2 * sizeof(glm::vec2)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);

    mXPositions = { -3.0f, -1.0f, 1.0f, 3.0f };

    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mRotations.push_back(glm::mat4(1.0f));
    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", gGameVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    currShader->SetUniform("uXPositions", mXPositions);
    currShader->SetUniform("uRotations", mRotations);

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_NOTES);

    int vboCount = getActiveVBOCount(mVAO);
    bool check0 = isAttributeEnabled(mVAO, 0);
    bool check1 = isAttributeEnabled(mVAO, 1);
    bool check2 = isAttributeEnabled(mVAO, 2);
    bool check3 = isAttributeEnabled(mVAO, 3);
    bool check4 = isAttributeEnabled(mVAO, 4);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::UpdateParams()
{
    // @TODO: Note styles
    const std::pair<float, unsigned> coords[] = {
        { 0.0f,			 448 },
        { 1.0f / 2.0f,	 384 },
        { 1.0f / 3.0f,	 320 },
        { 2.0f / 3.0f,	 320 },
        { 1.0f / 4.0f,	 256 },
        { 3.0f / 4.0f,	 256 },
        { 1.0f / 6.0f,	 192 },
        { 5.0f / 6.0f,	 192 },
        { 1.0f / 8.0f,	 128 },
        { 3.0f / 8.0f,	 128 },
        { 5.0f / 8.0f,	 128 },
        { 7.0f / 8.0f,	 128 },
        { 1.0f / 12.0f,	 64 },
        { 5.0f / 12.0f,	 64 },
        { 7.0f / 12.0f,	 64 },
        { 11.0f / 12.0f, 64 },
        { 1.0f / 5.0f,	 0 }
    };

    const size_t noteCount = mChart->mNotes.size();
    auto it = mChart->mNotes.begin();
    for (unsigned i = 0; i < MAX_NOTES; i++)
    {
        if (i >= noteCount)
        {
            for (; i < MAX_NOTES; i++)
            {
                unsigned floatIdx = i * 3;
                mFloatParams[floatIdx].x = -1.0f;
            }

            return;
        }

        unsigned floatIdx = i * 3;
        Note* note = *it;

        if (dynamic_cast<MineNote*>(note) != nullptr)
        {
            mFloatParams[floatIdx].x = -1.0f;
            it++;
            continue;
        }


        const float pos = note->mPos;
        const float decimalPart = pos - static_cast<int>(pos);
        unsigned currbeatUVOffset;
        for (unsigned idx = 0; idx < 8; idx++)
        {
            currbeatUVOffset = coords[idx].second;
            if (coords[idx].first == decimalPart)
                break;
        }

        mFloatParams[floatIdx]     = glm::vec2(note->mDir, pos);
        mFloatParams[floatIdx + 1] = SetTextureScale(64, 64);
        mFloatParams[floatIdx + 2] = SetTextureOffset(0, currbeatUVOffset);

        it++;
    }
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::UpdateVBOs()
{
    UpdateParams();

    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_NOTES * 3 * sizeof(glm::vec2), &mFloatParams[0]);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

HoldNoteBodyRenderer::HoldNoteBodyRenderer(Chart* inChart) : mChart(inChart)
{
    SetShader("engine/shaders/HoldNoteBodyRenderer.shader");
    mTexture = ResourceMgr->Load<Texture>("noteskins/USWCelSM5/Down Hold Body Inactive.png");
    mRollNoteTexture = ResourceMgr->Load<Texture>("noteskins/USWCelSM5/Down Roll Body Inactive.png");
    mHoldBottomCapTexture = ResourceMgr->Load<Texture>("noteskins/USWCelSM5/Down Hold BottomCap inactive.png");
    mRollBottomCapTexture = ResourceMgr->Load<Texture>("noteskins/USWCelSM5/Down Roll BottomCap Inactive.png");

    UpdateParams();

    GLenum error = glGetError();

    mVAO = GfxMgr->CreateQuadModel();
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mFloatVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_NOTES * 4 * sizeof(float), &mFloatParams[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void HoldNoteBodyRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", gGameVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uHoldNote", 1);

    glActiveTexture(GL_TEXTURE2);
    mHoldBottomCapTexture->get()->Bind();
    currShader->SetUniform("uHoldBottomCap", 2);

    glActiveTexture(GL_TEXTURE3);
    mRollNoteTexture->get()->Bind();
    currShader->SetUniform("uRollNote", 3);

    glActiveTexture(GL_TEXTURE4);
    mRollBottomCapTexture->get()->Bind();
    currShader->SetUniform("uRollBottomCap", 4);

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_NOTES);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void HoldNoteBodyRenderer::UpdateParams()
{
    std::multiset<HoldNote*, NoteCompare> holdNotes = mChart->GetAllHoldNotes();
    const float xPositions[4] = { -3.0f, -1.0f, 1.0f, 3.0f };

    const size_t noteCount = holdNotes.size();
    auto it = holdNotes.begin();
    for (unsigned i = 0; i < MAX_NOTES; i++)
    {
        if (i >= noteCount)
        {
            for (; i < MAX_NOTES; i++)
            {
                unsigned floatIdx = i * 4;
                mFloatParams[floatIdx] = -1.0f;
            }

            return;
        }

        HoldNote* currNote = *it;
        const bool isARollNote = dynamic_cast<RollNote*>(currNote) != nullptr;

        const float middlePos = (currNote->mPos + currNote->mEnd) / 2.0f;
        const float size = currNote->mEnd - currNote->mPos;

        unsigned floatIdx = i * 4;
        mFloatParams[floatIdx] = static_cast<float>(isARollNote);
        mFloatParams[floatIdx + 1] = xPositions[currNote->mDir];
        mFloatParams[floatIdx + 2] = middlePos;
        mFloatParams[floatIdx + 3] = size;

        it++;
    }
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void HoldNoteBodyRenderer::UpdateVBOs()
{
    UpdateParams();

    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_NOTES * 4 * sizeof(glm::vec2), &mFloatParams[0]);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

MineRenderer::MineRenderer(Chart* inChart) : mChart(inChart)
{
    SetShader("engine/shaders/MineRenderer.shader");
    mTexture = ResourceMgr->Load<Texture>("noteskins/USWCelSM5/_Down Tap Mine 8x1.png");

    UpdateParams();

    mVAO = GfxMgr->CreateQuadModel();
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mFloatVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_NOTES * 3 * sizeof(glm::vec2), &mFloatParams[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec2), (void*)(2 * sizeof(glm::vec2)));

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);

    mXPositions = { -3.0f, -1.0f, 1.0f, 3.0f };
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void MineRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", gGameVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    currShader->SetUniform("uXPositions", mXPositions);

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_NOTES);
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void MineRenderer::UpdateParams()
{
    std::multiset<MineNote*, NoteCompare> mineNotes = mChart->GetAllMineNotes();
    const size_t noteCount = mineNotes.size();
    auto it = mineNotes.begin();
    for (unsigned i = 0; i < MAX_NOTES; i++)
    {
        if (i >= noteCount)
        {
            for (; i < MAX_NOTES; i++)
            {
                unsigned floatIdx = i * 3;
                mFloatParams[floatIdx].x = -1.0f;
            }

            return;
        }

        unsigned floatIdx = i * 3;
        Note* note = *it;

        mFloatParams[floatIdx] = glm::vec2(note->mDir, note->mPos);
        mFloatParams[floatIdx + 1] = SetTextureScale(64, 64);
        mFloatParams[floatIdx + 2] = SetTextureOffset(192, 0);

        it++;
    }
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void MineRenderer::UpdateVBOs()
{
    UpdateParams();

    glBindBuffer(GL_ARRAY_BUFFER, mFloatVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_NOTES * 3 * sizeof(glm::vec2), &mFloatParams[0]);
}

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

    currShader->SetUniform("uTextColor", glm::vec3(mColor.x, mColor.y, mColor.z));
    currShader->SetUniform("zLayer", transform.pos.z);
    
    glActiveTexture(GL_TEXTURE0); 
    mFont->get()->BindFontTexture();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glm::uvec2 textSize = GetTextSize();
    int workingIndex = 0;
    float x = transform.pos.x - textSize.x / 2;
    float y = transform.pos.y - textSize.y / 2;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = mText.begin(); c != mText.end(); c++)
    {
        Character ch = pFont->mCharacters[*c];

        if (*c == '\n')
        {
            y -= ((ch.Size.y)) * 1.3 * transform.scale.y * FONTRENDERER_TEXTDEFAULTSIZE;
            x = transform.pos.x;
        }
        else if (*c == ' ') {
            x += (ch.Advance >> 6) * transform.scale.x * FONTRENDERER_TEXTDEFAULTSIZE;
        }
        else {
            float xpos = x + ch.Bearing.x * transform.scale.x * FONTRENDERER_TEXTDEFAULTSIZE;
            float ypos = y - (256 - ch.Bearing.y) * transform.scale.y * FONTRENDERER_TEXTDEFAULTSIZE;

            mTransforms[workingIndex] = glm::translate(glm::mat4(1.0f), glm::vec3(xpos, ypos, transform.pos.z + workingIndex * 0.0001f)) *
                                        glm::scale(glm::mat4(1.0f), glm::vec3(256 * transform.scale.x * FONTRENDERER_TEXTDEFAULTSIZE, 256 * transform.scale.y * FONTRENDERER_TEXTDEFAULTSIZE, 0));
            mLetterMap[workingIndex] = ch.TextureID;
            
            // render quad
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * transform.scale.x * FONTRENDERER_TEXTDEFAULTSIZE; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
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

void FontRenderer::SetText(const std::string& text)
{
    mText = text;
}

void FontRenderer::InitializeVertexData()
{
   const GLfloat vertex_data[] = 
   {
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,1.0f,
        1.0f,0.0f,
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

    std::string::const_iterator c;
    for (c = mText.begin(); c != mText.end(); c++)
    {
        Character ch = pFont->mCharacters[*c];

        if (*c == '\n')
        {
            totalSize.y -= ((ch.Size.y)) * 1.3 * transform.scale.y * FONTRENDERER_TEXTDEFAULTSIZE;
            totalSize.x = std::max(totalSize.x, currX);
            currX = transform.pos.x;
        }
        else
        {
            currX += (ch.Advance >> 6) * transform.scale.x * FONTRENDERER_TEXTDEFAULTSIZE;
        }
    }

    totalSize.x = std::max(totalSize.x, currX);
    return totalSize;
}
