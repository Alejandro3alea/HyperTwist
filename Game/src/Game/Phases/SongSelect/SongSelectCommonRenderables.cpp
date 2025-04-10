#include "SongSelectCommonRenderables.h"

SongSelectCommonRenderables::SongSelectCommonRenderables()
{
    SetTextures();
    SetPositions();
}

void SongSelectCommonRenderables::SetTextures()
{
    mStageText.SetFont("engine/fonts/Impact.ttf");
    mStageText.SetText("1st STAGE");
    mNote.SetTexture("engine/texture/SongSelect/ImportantNote.png");
}

void SongSelectCommonRenderables::SetPositions()
{
    // Creepily staring at these values (maybe @TODO for making a UI file IO system?)
    mStageText.transform.pos = glm::vec3(-820.0f, 465.0f, 3.0f);
    mStageText.transform.scale = glm::vec3(1.0f);
    mStageBG.transform.pos = glm::vec3(-825.0f, 490.0f, 2.5f);
    mStageBG.mColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);
    mStageBG.transform.scale = glm::vec3(150.0f, 50.0f, 0.1f);

    glm::ivec2 noteSize = mNote.mTexture->get()->GetSize();
    mNote.transform.pos = glm::vec3(0.0f, -450.0f, 20.0f);
    mNote.transform.scale = glm::vec3(noteSize.x, noteSize.y, 1.0f) * 0.65f;
}
