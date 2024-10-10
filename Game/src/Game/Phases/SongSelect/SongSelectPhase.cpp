#include "SongSelectPhase.h"
#include "Graphics/GfxMgr.h"

void SongSelectPhase::OnEnter()
{
	GfxMgr->SetBackgroundShader(ResourceMgr->Load<Shader>("data/shaders/SongSelectBG.shader")); 
}

void SongSelectPhase::OnTick(const float dt)
{
}

void SongSelectPhase::OnExit()
{
}
