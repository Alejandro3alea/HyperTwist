#include "MineRenderer.h"
#include "Audio/AudioMgr.h"
#include "Game/Chart.h"
#include "Game/GlobalVariables.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/Shader.h"

#include <set>

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

MineRenderer::MineRenderer(Chart* inChart) : TextureAtlas(), mChart(inChart) { Initialize(); }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

MineRenderer::MineRenderer(Chart* inChart, RenderPass* renderPass) : TextureAtlas(renderPass), mChart(inChart)
{
    Initialize();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void MineRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);

    glActiveTexture(GL_TEXTURE1);
    mTexture->get()->Bind();
    currShader->SetUniform("uTexture", 1);

    currShader->SetUniform("uXPositions", mXPositions);

    currShader->SetUniform("uSongOffset", mChart->mSong->GetPositionFromMusicTime(AudioMgr->GetMusicTime()));

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
                const size_t float_params_idx = i * 3;
                mFloatParams[float_params_idx].x = -1.0f;
            }

            return;
        }

        const size_t float_params_idx = i * 3;
        Note* note = *it;

        mFloatParams[float_params_idx] = glm::vec2(note->mDir, note->mMeasurePos);
        mFloatParams[float_params_idx + 1] = SetTextureScale(64, 64);
        mFloatParams[float_params_idx + 2] = SetTextureOffset(192, 0);

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

void MineRenderer::Initialize()
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

    mXPositions = {-3.0f, -1.0f, 1.0f, 3.0f};
}
