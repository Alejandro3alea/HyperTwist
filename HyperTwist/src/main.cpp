#pragma comment(lib, "SHELL32.LIB")

#include "Resources/ResourceMgr.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/GfxMgr.h"
#include "Input/InputMgr.h"
#include "Editor/Editor.h"
#include "Misc/Timer.h"

#include "Editor/Tools/Profiler/Profiler.h"

#include <imgui/imgui_impl_sdl.h>

#undef main
int main(void)
{
	ResourceMgr->Initialize();
	WindowMgr->Initialize("HyperTwist");
	GfxMgr->Initialize();
	//AudioMgr->Initialize();
	Editor->Initialize();
	InputMgr->Initialize();

	Editor->Load();

	while (!WindowMgr->mCurrentWindow->IsClosed())
	{
		TimeMgr->StartFrame();

		Profiler->FrameStart();

		Profiler->ProfileStart("Input");
		SDL_Event sdlEvent = InputMgr->ProcessInput();
		// @TODO: On Input Process End event
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		Profiler->ProfileEnd("Input");

		Profiler->ProfileStart("Graphics");
		GfxMgr->Render();
		Profiler->ProfileEnd("Graphics");

		Profiler->ProfileStart("Editor");
		Editor->Update(TimeMgr->deltaTime);
		Profiler->ProfileEnd("Editor");

		Profiler->ProfileStart("Swap");
		SDL_GL_SwapWindow(*WindowMgr->mCurrentWindow);
		Profiler->ProfileEnd("Swap");

		Profiler->FrameEnd();

		TimeMgr->EndFrame();
	}

	InputMgr->Shutdown();
	Editor->Shutdown();
	WindowMgr->Shutdown();
	//AudioMgr->Shutdown();
	GfxMgr->Shutdown();
	ResourceMgr->Shutdown();

	return 0;
}