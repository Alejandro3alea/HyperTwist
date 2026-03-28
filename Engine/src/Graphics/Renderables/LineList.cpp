#include "LineList.h"
#include "Audio/AudioMgr.h"
#include "Game/Chart.h"
#include "Game/GlobalVariables.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LineList::LineList(Chart* inChart) : IRenderable(), mChart(inChart) { Initialize(inChart); }

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

LineList::LineList(Chart* inChart, RenderPass* renderPass) : IRenderable(renderPass), mChart(inChart)
{
    Initialize(inChart);
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
    currShader->SetUniform("uZoom", gGlobalVariables.mZoom);
    if (mChart)
        currShader->SetUniform("uSongOffset", mChart->mSong->GetPositionFromMusicTime(AudioMgr->GetMusicTime()));

    glBindVertexArray(mVAO);

    // Draw the lines
    glDrawArrays(GL_LINES, 0, mLineCount);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void LineList::Initialize(Chart* inChart)
{
    std::vector<glm::vec2> positions = ComputeLinePositions(inChart);

    mLineCount = positions.size() / 2;
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

std::vector<glm::vec2> LineList::ComputeLinePositions(Chart* inChart)
{
    if (inChart == nullptr)
        return std::vector<glm::vec2>();

    const size_t lastMeasure = std::ceil(inChart->GetMeasureFromLastNote() * 2) + 4;
    std::vector<glm::vec2> positions;
    for (unsigned i = 0; i < lastMeasure; i++)
    {
        int repetitions = (i % 4 == 0) ? 4 : 1;

        for (unsigned j = 0; j < repetitions; j++)
        {
            positions.push_back({-4.0f, i});
            positions.push_back({4.0f, i});
        }
    }

    return positions;
}
