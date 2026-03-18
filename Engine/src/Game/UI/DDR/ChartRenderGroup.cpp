#include "ChartRenderGroup.h"

ChartRenderGroup::ChartRenderGroup(Chart* inChart)
{
    mDarkBackground = std::make_unique<Renderable>();
    mDarkBackground->SetShader("engine/shaders/NotesUI.shader");
    mDarkBackground.get()->mColor = {0.0f, 0.0f, 0.0f, 0.75f};
    mDarkBackground.get()->transform.pos = {0.0f, 0.0f, 0.1f};
    mDarkBackground.get()->transform.scale = {280.0f, 1000.0f, 0.05f};

    mMeasureLines = std::make_unique<LineList>(inChart);

    mNoteRenderer = std::make_shared<NoteRenderer>(inChart);
    mHoldRenderer = std::make_shared<HoldNoteBodyRenderer>(inChart);
    // mMineRenderer[i] = std::make_shared<MineRenderer>(inChart);
    mPlayerReceptor = std::make_shared<Receptors>();

    mNoteRenderer->mTextureScale = glm::vec2(0.05f);
    mHoldRenderer->mTextureScale = glm::vec2(3.0f);
    mNoteRenderer->mTextureOffset = glm::vec2(0.0f);
    mHoldRenderer->mTextureOffset = glm::vec2(0.0f);
    // mMineRenderer->mTextureScale = glm::vec3(300.0f);

    mNoteRenderer->mbIsVisible = true;
    mHoldRenderer->mbIsVisible = true;
    // mMineRenderer->mbIsVisible = true;

    mPlayerReceptor->Initialize();
}