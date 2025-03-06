#include "WindowMgr.h"
#include "Misc/ColorConsole.h"
#include "Input/InputMgr.h"
#include "Misc/Requires.h"

WindowManager* WindowManager::mpInstance;

Window::Window(const std::string& winName, const unsigned winWidth, const unsigned winHeight) : mSize(winWidth, winHeight)
{
	// Create an instance of an SDLWindow
	mWindow_SDL = SDL_CreateWindow(winName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (mWindow_SDL == nullptr)
	{
		PrintColoredText(std::string("SDL_CreateWindow Error : ") + SDL_GetError(), ConsoleColor::Red);
		SDL_Quit();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Create an SDL context for OpenGL
	mWinContext = SDL_GL_CreateContext(mWindow_SDL);
	if (mWinContext == nullptr)
	{
		SDL_DestroyWindow(mWindow_SDL);
		PrintColoredText(std::string("SDL_GL_CreateContext Error : ") + SDL_GetError(), ConsoleColor::Red);
		SDL_Quit();
		exit(1);
	}

	Requires(SDL_GL_SetSwapInterval(1) >= 0, "Failed to enable V-Sync.");

	mAspectRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);

	//SDL_GL_SetSwapInterval(0);
	SDL_MaximizeWindow(mWindow_SDL);
}

Window::~Window()
{
	SDL_GL_DeleteContext(mWinContext);
	SDL_DestroyWindow(mWindow_SDL);
	SDL_Quit();
}

void WindowManager::Initialize(const std::string& windowName)
{
	// Initialize SLD library
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		PrintColoredText(std::string("Could not initialize SDL : ") + SDL_GetError(), ConsoleColor::Red);
		exit(1);
	}

	mCurrentWindow = std::shared_ptr<Window>(new Window(windowName, 1600, 900));

	InputMgr->mOnWindowResize.Add([this](const uint32_t width, const uint32_t height) {mCurrentWindow->Resize(glm::uvec2(width, height)); });
}

void WindowManager::Update()
{

}

void WindowManager::Shutdown()
{
}
