#include "HoldNoteBodyRenderer.h"
#include "Audio/AudioMgr.h"
#include "Game/Chart.h"
#include "Game/GlobalVariables.h"
#include "Graphics/GfxMgr.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

HoldNoteBodyRenderer::HoldNoteBodyRenderer(Chart* inChart) : TextureAtlas(), mChart(inChart) { Initialize(); }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

HoldNoteBodyRenderer::HoldNoteBodyRenderer(Chart* inChart, RenderPass* renderPass)
    : TextureAtlas(renderPass), mChart(inChart)
{
    Initialize();
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void HoldNoteBodyRenderer::Render(Shader* shader)
{
    Shader* currShader = (shader != nullptr) ? shader : mShader->get();
    currShader->Bind();

    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);

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

    currShader->SetUniform("uSongOffset", mChart->mSong->GetPositionFromMusicTime(AudioMgr->GetMusicTime()));

    glBindVertexArray(mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, MAX_NOTES);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void HoldNoteBodyRenderer::UpdateParams()
{
    std::multiset<HoldNote*, NoteCompare> holdNotes = mChart->GetAllHoldNotes();
    const float xPositions[4] = {-3.0f, -1.0f, 1.0f, 3.0f};

    const size_t noteCount = holdNotes.size();
    auto it = holdNotes.begin();
    for (unsigned i = 0; i < MAX_NOTES; i++)
    {
        if (i >= noteCount)
        {
            for (; i < MAX_NOTES; i++)
            {
                const size_t float_params_idx = i * 4;
                mFloatParams[float_params_idx] = -1.0f;
            }

            return;
        }

        HoldNote* currNote = *it;
        const bool isARollNote = dynamic_cast<RollNote*>(currNote) != nullptr;

        const float middlePos = (currNote->mMeasurePos + currNote->mEnd) / 2.0f;
        const float size = currNote->mEnd - currNote->mMeasurePos;

        const size_t float_params_idx = i * 4;
        mFloatParams[float_params_idx] = static_cast<float>(isARollNote);
        mFloatParams[float_params_idx + 1] = xPositions[currNote->mDir];
        mFloatParams[float_params_idx + 2] = middlePos;
        mFloatParams[float_params_idx + 3] = size;

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

void HoldNoteBodyRenderer::Initialize()
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
