#pragma comment(lib, "SHELL32.LIB")

#include "Game/Phases/PhaseManager.h"
#include "Resources/ResourceMgr.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/GfxMgr.h"
#include "Input/InputMgr.h"
#include "Misc/Timer.h"

#undef main
int main(void)
{
	ResourceMgr->Initialize();
	WindowMgr->Initialize("Nextage");
	GfxMgr->Initialize();
	ResourceMgr->LoadDefaultAssets();
	InputMgr->Initialize();
	PhaseMgr->Initialize();


	PhaseMgr->ChangeToScene("SongSelect");

	while (!WindowMgr->mCurrentWindow->IsClosed())
	{
		TimeMgr->StartFrame();

		InputMgr->ProcessInput();
		PhaseMgr->Update(TimeMgr->deltaTime);

		GfxMgr->Render();
		SDL_GL_SwapWindow(*WindowMgr->mCurrentWindow);

		TimeMgr->EndFrame();
	}

	InputMgr->Shutdown();
	WindowMgr->Shutdown();
	//AudioMgr->Shutdown();
	GfxMgr->Shutdown();
	ResourceMgr->Shutdown();

	return 0;
}