#include "NoteRenderer.h"
#include "Audio/AudioMgr.h"
#include "Game/Chart.h"
#include "Game/GlobalVariables.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

NoteRenderer::NoteRenderer(Chart* inChart) : TextureAtlas(), mChart(inChart) { Initialize(); }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

NoteRenderer::NoteRenderer(Chart* inChart, RenderPass* renderPass) : TextureAtlas(renderPass), mChart(inChart)
{
    Initialize();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    currShader->SetUniform("uXPositions", mXPositions);
    currShader->SetUniform("uRotations", mRotations);

    currShader->SetUniform("uNoteScale", 1.0f);
    currShader->SetUniform("uSongOffset", mChart->mSong->GetPositionFromMusicTime(AudioMgr->GetMusicTime()));

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_NOTES);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::Initialize()
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

    mXPositions = {-180.0f, -60.0f, 60.0f, 180.0f};

    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mRotations.push_back(glm::mat4(1.0f));
    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mRotations.push_back(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void NoteRenderer::UpdateParams()
{
    // @TODO: Note styles
    const std::pair<float, unsigned> coords[] = {
        {0.0f, 448},         {1.0f / 2.0f, 384}, {1.0f / 3.0f, 320}, {2.0f / 3.0f, 320}, {1.0f / 4.0f, 256},
        {3.0f / 4.0f, 256},  {1.0f / 6.0f, 192}, {5.0f / 6.0f, 192}, {1.0f / 8.0f, 128}, {3.0f / 8.0f, 128},
        {5.0f / 8.0f, 128},  {7.0f / 8.0f, 128}, {1.0f / 12.0f, 64}, {5.0f / 12.0f, 64}, {7.0f / 12.0f, 64},
        {11.0f / 12.0f, 64}, {1.0f / 5.0f, 0}};

    const size_t noteCount = mChart->mNotes.size();
    auto it = mChart->mNotes.begin();
    // @TODO: Do rendering in batches if note overflow
    for (unsigned i = 0; i < MAX_NOTES; i++)
    {
        if (i >= noteCount)
        {
            for (; i < MAX_NOTES; i++)
            {
                const size_t float_params_idx = i * 3;
                mFloatParams[float_params_idx].x = -1.0f;
            }

            return;
        }

        const size_t float_params_idx = i * 3;
        Note* note = *it;

        if (dynamic_cast<MineNote*>(note) != nullptr)
        {
            mFloatParams[float_params_idx].x = -1.0f;
            it++;
            continue;
        }

        const float pos = note->mMeasurePos;
        const float decimalPart = pos - static_cast<int>(pos);
        unsigned currbeatUVOffset;
        for (unsigned idx = 0; idx < 8; idx++)
        {
            currbeatUVOffset = coords[idx].second;
            if (coords[idx].first == decimalPart)
                break;
        }

        mFloatParams[float_params_idx] = glm::vec2(note->mDir, pos);
        mFloatParams[float_params_idx + 1] = SetTextureScale(64, 64);
        mFloatParams[float_params_idx + 2] = SetTextureOffset(0, currbeatUVOffset);

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
