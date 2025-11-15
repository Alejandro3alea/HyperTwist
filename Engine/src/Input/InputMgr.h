#pragma once
#include "Composition/Events/Event.h"
#include "Misc/DataTypes.h"
#include "Misc/Singleton.h"

#include "InputAction.h"
#include <SDL3/SDL.h>
#include <boost/bimap.hpp>
#include <glm/glm.hpp>

CREATE_MULTICAST_EVENT(OnPostInputProcess, SDL_Event);
CREATE_MULTICAST_EVENT(OnWindowResize, u32, u32);

struct MouseState
{
    u32 mMouseMask[5];
    glm::ivec2 mPos;
};

class InputManager
{
    Singleton(InputManager)

        using KeyboardState = u8;

    enum eMouseButton
    {
        eButtonLeft = 0,
        eButtonMiddle = 1,
        eButtonRight = 2,
        eButtonX1 = 3,
        eButtonX2 = 4
    };

    void Initialize();
    void Shutdown();

    bool isKeyPressed(SDL_Scancode key); // @TODO: Does not work
    bool isKeyReleased(SDL_Scancode key);
    bool isKeyDown(SDL_Scancode key);
    bool isKeyUp(SDL_Scancode key);

    bool isButtonPressed(eMouseButton button);
    bool isButtonReleased(eMouseButton button);
    bool isButtonDown(eMouseButton button);
    bool isButtonUp(eMouseButton button);

    glm::ivec2 GetMouseMovement() { return mMouseMovement; }
    glm::ivec2 GetMousePos() { return mMouseState.mPos; }

    void SetMousePos(const glm::ivec2& pos)
    {
        mMouseState.mPos = pos;
        SDL_WarpMouseGlobal(static_cast<f32>(pos.x), static_cast<f32>(pos.y));
    }
    void SetMousePos(const i32 w, const i32 h)
    {
        mMouseState.mPos = glm::ivec2(w, h);
        SDL_WarpMouseGlobal(static_cast<f32>(w), static_cast<f32>(h));
    }

    SDL_Event ProcessInput();

  private:
    void ResetToDefaultMappings();

    bool DoesMappingsJsonPathExist();
    void LoadMappingsFromJson();
    void SaveMappingsToJson();

  public:
    OnPostInputProcess mOnPostInputProcess;
    OnWindowResize mOnWindowResize;

  private:
    enum eInputState
    {
        eInputUp = 0,           // 0000
        eInputDown = 1 << 0,    // 0001
        eInputPressed = 1 << 1, // 0010
        eInputReleased = 1 << 2 // 0100
    };

    KeyboardState mKeyboardState[SDL_SCANCODE_COUNT];
    MouseState mMouseState;
    glm::ivec2 mMouseMovement;

    boost::bimap<InputAction, SDL_Scancode> mMappings;

    u8* mCurrentKeyState = nullptr;
    u32 mCurrentButtonState;
};

#define InputMgr InputManager::Instance()