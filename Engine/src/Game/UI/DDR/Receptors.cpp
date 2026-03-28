#include "Receptors.h"
#include "Game/GlobalVariables.h"
#include "Resources/ResourceMgr.h"

Receptors::Receptors(RenderPass* renderPass)
{
    constexpr u8 DirCount = 4;
    for (u8 i = 0; i < DirCount; i++)
    {
        mReceptorOff[i] = std::make_unique<TextureAtlas>(renderPass);
        mReceptorOn[i] = std::make_unique<TextureAtlas>(renderPass);
    }
}

void Receptors::Initialize()
{
    constexpr u8 DirCount = 4;
    constexpr f32 noteDefaultSize = 60.0f;
    float noteInbetweenOffset = -3.0f;
    const float rotations[DirCount] = {-90.0f, 0.0f, 180.0f, 90.0f};
    for (unsigned i = 0; i < DirCount; i++)
    {
        TextureAtlas& currReceptorOn = *mReceptorOn[i];
        TextureAtlas& currReceptorOff = *mReceptorOff[i];
        mReceptorOn[i]->mShader = currReceptorOff.mShader =
            ResourceMgr->Load<Shader>("engine/shaders/Receptors.shader");
        // @TODO
        currReceptorOn.mTexture = currReceptorOff.mTexture =
            ResourceMgr->Load<Texture>("noteskins/USWCelSM5/_Down Receptor Go 4x1.png");
        currReceptorOn.SetTextureScale({64, 64});
        currReceptorOn.SetTextureOffset({64, 0});
        currReceptorOff.SetTextureScale({64, 64});
        currReceptorOff.SetTextureOffset({64, 0});
        currReceptorOn.transform.rotation = currReceptorOff.transform.rotation = rotations[i];
        currReceptorOn.transform.pos.x = currReceptorOff.transform.pos.x =
            noteInbetweenOffset * gGlobalVariables.mZoom * noteDefaultSize / 2.0f;
        currReceptorOn.transform.pos.y = currReceptorOff.transform.pos.y = -400.0f;
        currReceptorOn.transform.pos.z = currReceptorOff.transform.pos.z = 0.5f;
        currReceptorOn.transform.scale = currReceptorOff.transform.scale = {noteDefaultSize, noteDefaultSize, 1.0f};
        noteInbetweenOffset += 2.0f;
    }
}

void Receptors::Update(const glm::vec3& camPos)
{
    float offset = gGlobalVariables.ReceptorsOffset * gGlobalVariables.Inverse;
    for (unsigned i = 0; i < 4; i++)
    {
        mReceptorOn[i]->transform.pos.y = mReceptorOff[i]->transform.pos.y = camPos.y * gGlobalVariables.mZoom + offset;
    }
}
