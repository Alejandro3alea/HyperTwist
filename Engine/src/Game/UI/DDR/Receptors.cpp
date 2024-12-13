#include "Receptors.h"
#include "Game/GameVariables.h"

void Receptors::Initialize()
{
	float offset = -3.0f;
	const float rotations[4] = { -90.0f, 0.0f, 180.0f, 90.0f };
	for (unsigned i = 0; i < 4; i++)
	{
		mReceptorOn[i].mShader = mReceptorOff[i].mShader = ResourceMgr->LoadFromBasePath<Shader>("data/engine/shaders/Receptors.shader");
		// @TODO
		mReceptorOn[i].mTexture = mReceptorOff[i].mTexture = ResourceMgr->LoadFromBasePath<Texture>("data/noteskins/USWCelSM5/_Down Receptor Go 4x1.png");
		mReceptorOn[i].SetTextureScale({64, 64});
		mReceptorOn[i].SetTextureOffset({ 64, 0 });
		mReceptorOff[i].SetTextureScale({ 64, 64 });
		mReceptorOff[i].SetTextureOffset({ 64, 0 });
		mReceptorOn[i].transform.rotation	= mReceptorOff[i].transform.rotation = rotations[i];
		mReceptorOn[i].transform.pos.x		= mReceptorOff[i].transform.pos.x	= offset * gGameVariables.mZoom;
		offset += 2.0f;
	}
}

void Receptors::Update(const glm::vec3& camPos)
{
	float offset = gGameVariables.ReceptorsOffset * gGameVariables.Inverse;
	for (unsigned i = 0; i < 4; i++)
	{
		mReceptorOn[i].transform.pos.y = mReceptorOff[i].transform.pos.y = camPos.y * gGameVariables.mZoom + offset;
	}
}