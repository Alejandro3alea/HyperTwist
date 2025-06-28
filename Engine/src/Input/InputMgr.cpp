#include "InputMgr.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/GfxMgr.h"

InputManager* InputManager::mpInstance;

void InputManager::Initialize()
{
	int keyboardSize;
	SDL_GetKeyboardState(&keyboardSize);
	mCurrentKeyState = new u8[keyboardSize];
}

void InputManager::Shutdown()
{
	delete[] mCurrentKeyState;
}

bool InputManager::isKeyPressed(SDL_Scancode key)
{
	return mKeyboardState[key] & eInputState::eInputPressed;
}

bool InputManager::isKeyReleased(SDL_Scancode key)
{
	return mKeyboardState[key] & eInputState::eInputReleased;
}

bool InputManager::isKeyDown(SDL_Scancode key)
{
	return mKeyboardState[key] & eInputState::eInputDown;
}

bool InputManager::isKeyUp(SDL_Scancode key)
{
	return mKeyboardState[key] & eInputState::eInputUp;
}

bool InputManager::isButtonPressed(eMouseButton button)
{
	return mMouseState.mMouseMask[button] & eInputState::eInputPressed;
}

bool InputManager::isButtonReleased(eMouseButton button)
{
	return mMouseState.mMouseMask[button] & eInputState::eInputReleased;
}

bool InputManager::isButtonDown(eMouseButton button)
{
	return mMouseState.mMouseMask[button] & eInputState::eInputDown;
}

bool InputManager::isButtonUp(eMouseButton button)
{
	return mMouseState.mMouseMask[button] & eInputState::eInputUp;
}

SDL_Event InputManager::ProcessInput()
{
	//--------------------------------- EVENTS ---------------------------------
	SDL_Event event_sdl;

	
	while (SDL_PollEvent(&event_sdl))
	{
		u32 newWidth; 
		u32 newHeight;

		switch (event_sdl.type)
		{
		case SDL_EVENT_WINDOW_RESIZED:
			newWidth = event_sdl.window.data1;
			newHeight = event_sdl.window.data2;
			mOnWindowResize.Broadcast(newWidth, newHeight);
			break;


		[[unlikely]] case SDL_EVENT_QUIT:
			WindowMgr->mCurrentWindow->Close();
			break;

		default:
			break;
		}
	}

	//--------------------------------- KEYS ---------------------------------
	int keyboardSize;
	const bool* newKeyboardState = SDL_GetKeyboardState(&keyboardSize);

	for (int i = 0; i < keyboardSize; i++)
	{
		mKeyboardState[i] = 0;

		if (newKeyboardState[i])
			mKeyboardState[i] |= eInputState::eInputDown;	  // key up
		else
			mKeyboardState[i] |= eInputState::eInputUp;	      // key down

		if (!mCurrentKeyState[i] && newKeyboardState[i])
			mKeyboardState[i] |= eInputState::eInputPressed;  // key pressed
		else if (mCurrentKeyState[i] && !newKeyboardState[i])
			mKeyboardState[i] |= eInputState::eInputReleased; // key released

		mCurrentKeyState[i] = newKeyboardState[i];
	}

	//-------------------------------- BUTTONS ----------------------------------
	f32 x, y;
	const u32 newMouseState = SDL_GetMouseState(&x, &y);

	for (u32 i = 0; i < 5; i++) // for each mouse button
	{
		mMouseState.mMouseMask[i] = 0;

		bool buttonState = newMouseState & 1 << i;
		bool previousButtonState = mCurrentButtonState & 1 << i;

		if (!buttonState)
			mMouseState.mMouseMask[i] |= eInputState::eInputUp;	       // key up
		else
			mMouseState.mMouseMask[i] |= eInputState::eInputDown;	   // key down

		if (!previousButtonState && buttonState)
			mMouseState.mMouseMask[i] |= eInputState::eInputPressed;  // key pressed
		else if (previousButtonState && buttonState)
			mMouseState.mMouseMask[i] |= eInputState::eInputReleased; // key released
	}

	mCurrentButtonState = newMouseState;

	mMouseMovement = glm::ivec2(x - mMouseState.mPos.x, y - mMouseState.mPos.y);
	mMouseState.mPos = glm::ivec2(x, y);

	mOnPostInputProcess.Broadcast(event_sdl);
	return event_sdl;
}
