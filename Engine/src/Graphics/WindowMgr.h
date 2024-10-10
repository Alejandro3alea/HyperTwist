#pragma once
#include "Misc/Singleton.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <windows.h>
#include <memory>
#include <string>

struct Window
{
	Window(const std::string& winName, const unsigned winWidth, const unsigned winHeight);
	~Window();

	void Close() { mbIsClosed = true; }
	bool IsClosed() { return mbIsClosed; }

	glm::uvec2 Size() { return mSize; }
	float AspectRatio() { return mAspectRatio; }

	operator SDL_GLContext() const { return mWinContext; }
	operator SDL_Window*() const { return mWindow_SDL; }

	void Resize(glm::uvec2 newSize) { mSize = newSize; }

private:
	glm::uvec2 mSize;
	SDL_Window* mWindow_SDL;
	SDL_GLContext mWinContext;
	float mAspectRatio;

	bool mbIsClosed = false;
};

class WindowManager
{
	Singleton(WindowManager);

	void Initialize(const std::string& windowName);
	void Update();
	void Shutdown();

public:
	std::shared_ptr<Window> mCurrentWindow;
};

#define WindowMgr WindowManager::Instance()