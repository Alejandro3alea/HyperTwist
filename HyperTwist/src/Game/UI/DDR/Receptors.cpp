#include "Receptors.h"

void Receptors::Initialize()
{
	float offset = -3.0f;
	const float rotations[4] = { -90.0f, 0.0f, 180.0f, 90.0f };
	for (unsigned i = 0; i < 4; i++)
	{
		mReceptorOn[i].mTexture = ResourceMgr->Load<Texture>("data/noteskins/metal/tex notes.png");
		mReceptorOn[i].mTexture = mReceptorOff[i].mTexture = ResourceMgr->Load<Texture>("data/noteskins/metal/tex receptors.png");
		mReceptorOn[i].SetTextureScale({ 64, 64 });
		mReceptorOn[i].SetTextureOffset({ 0, 0 });
		mReceptorOff[i].SetTextureScale({ 64, 64 });
		mReceptorOff[i].SetTextureOffset({ 64, 0 });
		mReceptorOn[i].transform.rotation	= mReceptorOff[i].transform.rotation = rotations[i];
		mReceptorOn[i].transform.pos.x		= mReceptorOff[i].transform.pos.x	= offset;
		offset += 2.0f;
	}
}

void Receptors::Update()
{

}
