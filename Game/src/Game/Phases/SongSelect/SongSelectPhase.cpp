#include "SongSelectPhase.h"
#include "Graphics/GfxMgr.h"

void SongSelectPhase::OnEnter()
{
	//GfxMgr->SetBackgroundShader(ResourceMgr->Load<Shader>("data/shaders/SongSelectBG.shader")); 
	GfxMgr->SetBackgroundTexture(ResourceMgr->Load<Texture>("data/textures/NextageLogo.png"));
}

void SongSelectPhase::OnTick(const float dt)
{
	
}

void SongSelectPhase::OnExit()
{
}
