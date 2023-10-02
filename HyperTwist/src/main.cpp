#pragma comment(lib, "SHELL32.LIB")

#include "Resources/ResourceMgr.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/GfxMgr.h"
#include "Audio/AudioMgr.h"
#include "Input/InputMgr.h"
#include "Editor/Editor.h"
#include "Misc/Timer.h"

#include "Editor/Tools/Profiler/Profiler.h"

#define MAX_FPS 60
#define FRAME_DELAY 1.0f / MAX_FPS

#undef main
int main(void)
{
	ResourceMgr->Initialize();
	WindowMgr->Initialize();
	GfxMgr->Initialize();
	AudioMgr->Initialize();
	Editor->Initialize();
	InputMgr->Initialize();

	GfxMgr->Load();
	Editor->Load();

	while (!WindowMgr->mCurrentWindow->isClosed())
	{
		Time->StartFrame();

		Profiler->FrameStart();

		Profiler->ProfileStart("Input");
		InputMgr->ProcessInput();
		Profiler->ProfileEnd("Input");

		Profiler->ProfileStart("Graphics");
		GfxMgr->Update();
		GfxMgr->Render();
		Profiler->ProfileEnd("Graphics");

		Profiler->ProfileStart("Editor");
		Editor->Update(Time->deltaTime);
		Profiler->ProfileEnd("Editor");

		Profiler->ProfileStart("Audio");
		AudioMgr->Update();
		Profiler->ProfileEnd("Audio");

		Profiler->ProfileStart("Swap");
		SDL_GL_SwapWindow(*WindowMgr->mCurrentWindow);
		Profiler->ProfileEnd("Swap");

		Profiler->FrameEnd();

		Time->EndFrame();

		const float frameDelay = FRAME_DELAY;
		const float frameTime = Time->deltaTime;
		const float diff = FRAME_DELAY - Time->deltaTime;
		/*if (FRAME_DELAY > Time->deltaTime)
		{
			SDL_Delay(FRAME_DELAY - Time->deltaTime);
			Time->deltaTime = FRAME_DELAY;
		}*/
	}

	InputMgr->Shutdown();
	Editor->Shutdown();
	WindowMgr->Shutdown();
	AudioMgr->Shutdown();
	GfxMgr->Shutdown();
	ResourceMgr->Shutdown();

	return 0;
}