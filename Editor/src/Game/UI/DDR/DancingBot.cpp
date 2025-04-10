#include "DancingBot.h"
#include "Editor/Editor.h"
#include "Graphics/GfxMgr.h"
#include "Game/GlobalEvents.h"
#include "Graphics/WindowMgr.h"
#include "Composition/Transform.h"

DancingBot::DancingBot() : mLeftFoot(false), mRightFoot(true)
{
    ColorBuffer colBuf(WindowMgr->mCurrentWindow->Size());
    mFb = new Framebuffer({ &colBuf });
	mFb->Initialize(false);

    mPadTex = ResourceMgr->LoadFromBasePath<Texture>("engine/texture/TempPad.png");
    mLFootTex = ResourceMgr->LoadFromBasePath<Texture>("engine/texture/LFoot.png");
    mRFootTex = ResourceMgr->LoadFromBasePath<Texture>("engine/texture/RFoot.png");

    GlobalEvents::gOnSongUpdate.Add([this](const float currMeasure) { UpdateFeetPosition(currMeasure); });

    mLeftFoot.mOtherFoot = &mRightFoot;
    mRightFoot.mOtherFoot = &mLeftFoot;
}

void DancingBot::UpdateDancingTex()
{
	mFb->BindFramebuffer();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader* quadShader = GfxMgr->GetQuadShader();
    quadShader->Bind();
    glBindVertexArray(GfxMgr->GetQuad());
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    quadShader->SetUniform("view", view);
    quadShader->SetUniform("proj", proj);
    quadShader->SetUniform("uColor", glm::vec4(1.0f));
    quadShader->SetUniform("uTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    mPadTex->get()->Bind();

    const float third = 1.0f / 3.0f;
    const float posVal = 4.0f / 6.0f;
    const glm::vec2 upPos = { 0.0f, posVal };
    const glm::vec2 leftPos = { posVal, 0.0f };
    const glm::vec2 downPos = { 0.0f, -posVal };
    const glm::vec2 rightPos = { -posVal, 0.0f };
    const glm::vec2 scale = { third, third };
    const Transform transforms[4] =
    {
        { upPos,    scale, 0.0f   },
        { leftPos,  scale, 270.0f },
        { downPos,  scale, 180.0f },
        { rightPos, scale, 90.0f  }
    };

    for (unsigned i = 0; i < 4; i++)
    {
        quadShader->SetUniform("model", transforms[i].GetModelMtx());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    mLFootTex->get()->Bind();
    quadShader->SetUniform("model", mLeftFoot.mTr.GetModelMtx());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glActiveTexture(GL_TEXTURE0);
    mRFootTex->get()->Bind();
    quadShader->SetUniform("model", mRightFoot.mTr.GetModelMtx());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    mFb->UnbindFramebuffer();
}

void DancingBot::OnGui()
{
    UpdateDancingTex();

	if (ImGui::Begin("Dancing Bot") && mFb)
    {
        const float ySizeOffset = 0.94f;
        const glm::vec2 winSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y * ySizeOffset };
        const float minVal = (winSize.x < winSize.y) ? winSize.x : winSize.y;
		ImGui::Image(reinterpret_cast<ImTextureID>(mFb->GetTexID()), { minVal * 0.95f, minVal * 0.95f }, { 0,1 }, { 1,0 });
	}

	ImGui::End();
}

void DancingBot::UpdateFeetPosition(const float currMeasure)
{
    mLeftFoot.Update(currMeasure);
    mRightFoot.Update(currMeasure);
}

DancingBotFoot::DancingBotFoot(const bool footPosition)
{
    glm::vec3 leftPosition = { -0.2, 0.0f, 0.0f };
    glm::vec3 rightPosition = { 0.2, 0.0f, 0.0f };
    mTr.pos = mLastBeat.mPos = (footPosition == false) ? leftPosition : rightPosition;
    mTr.scale = { 0.3f, 0.3f, 1.0f };
}

void DancingBotFoot::Update(const float currMeasure)
{
    if (currMeasure >= mNextBeat.mMeasure)
        mOtherFoot->SetNextBeat(currMeasure);

    const float t = (currMeasure - mLastBeat.mMeasure) / (mNextBeat.mMeasure - mLastBeat.mMeasure);
    mTr.pos = glm::mix(mLastBeat.mPos, mNextBeat.mPos, t);

    // @TODO: Rotations
}

void DancingBotFoot::SetNextBeat(const float currMeasure)
{
    Note* tempNote = new Note(LeftDirection, currMeasure);
    auto& noteList = Editor->GetCurrNotes();
    auto measureNotes = noteList.upper_bound(tempNote);
    if (measureNotes != noteList.end())
    {
        const float posVal = 4.0f / 6.0f;
        const glm::vec3 positions[4] = {
            {posVal, 0.0f, 0.0f},    // Left
            { 0.0f, -posVal, 0.0f }, // Down
            { 0.0f, posVal, 0.0f },  // Up
            { -posVal, 0.0f, 0.0f }  // Right
        };

        Note* currNote = (*measureNotes);

        mOtherFoot->mNextBeat.mMeasure = currNote->mPos;
        mOtherFoot->mNextBeat.mPos = positions[currNote->mDir];
    }
    
    delete tempNote;
}
