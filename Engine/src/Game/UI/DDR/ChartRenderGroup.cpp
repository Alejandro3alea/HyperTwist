#include "ChartRenderGroup.h"

ChartRenderGroup::ChartRenderGroup(Chart* inChart, RenderPass* renderPass)
{
    mNotesBackground = std::make_unique<Renderable>(renderPass);
    mNotesBackground->SetShader("engine/shaders/NotesUI.shader");
    mNotesBackground->mColor = {0.0f, 0.0f, 0.0f, 0.75f};
    mNotesBackground->transform.pos = {0.0f, 0.0f, 0.1f};
    mNotesBackground->transform.scale = {280.0f, 1000.0f, 0.05f};

    mMeasureLines = std::make_unique<LineList>(inChart, renderPass);
    mMeasureLines->transform.pos.z = 1.25f;

    mNoteRenderer = std::make_shared<NoteRenderer>(inChart, renderPass);
    mNoteRenderer->transform.pos.z = 1.5f;

    mHoldRenderer = std::make_shared<HoldNoteBodyRenderer>(inChart, renderPass);
    mHoldRenderer->transform.pos.z = 1.4f;

    mMineRenderer = std::make_shared<MineRenderer>(inChart, renderPass);
    mMineRenderer->transform.pos.z = 1.45f;

    mPlayerReceptor = std::make_shared<Receptors>(renderPass);

    mNoteRenderer->mTextureScale = glm::vec2(0.05f);
    mHoldRenderer->mTextureScale = glm::vec2(3.0f);
    mNoteRenderer->mTextureOffset = glm::vec2(0.0f);
    mHoldRenderer->mTextureOffset = glm::vec2(0.0f);
    mMineRenderer->mTextureScale = glm::vec3(300.0f);

    mNoteRenderer->mbIsVisible = true;
    mHoldRenderer->mbIsVisible = true;
    mMineRenderer->mbIsVisible = true;

    mPlayerReceptor->Initialize();
}