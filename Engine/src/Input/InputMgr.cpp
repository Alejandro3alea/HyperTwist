#include "InputMgr.h"
#include "Graphics/GfxMgr.h"
#include "Graphics/WindowMgr.h"
#include "Utils/InputUtils.h"

#include <Json/json.hpp>
using json = nlohmann::json;

#include <fstream>

#define MAPPINGS_JSON_PATH FileUtils::JoinPath(DATA_PATH, "config/mappings.json")

InputManager* InputManager::mpInstance;

void InputManager::Initialize()
{
    int keyboardSize;
    SDL_GetKeyboardState(&keyboardSize);
    mCurrentKeyState = new u8[keyboardSize];
}

void InputManager::Shutdown() { delete[] mCurrentKeyState; }

bool InputManager::isKeyPressed(SDL_Scancode key) { return mKeyboardState[key] & eInputState::eInputPressed; }

bool InputManager::isKeyReleased(SDL_Scancode key) { return mKeyboardState[key] & eInputState::eInputReleased; }

bool InputManager::isKeyDown(SDL_Scancode key) { return mKeyboardState[key] & eInputState::eInputDown; }

bool InputManager::isKeyUp(SDL_Scancode key) { return mKeyboardState[key] & eInputState::eInputUp; }

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

bool InputManager::isButtonUp(eMouseButton button) { return mMouseState.mMouseMask[button] & eInputState::eInputUp; }

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
            mKeyboardState[i] |= eInputState::eInputDown; // key up
        else
            mKeyboardState[i] |= eInputState::eInputUp; // key down

        if (!mCurrentKeyState[i] && newKeyboardState[i])
            mKeyboardState[i] |= eInputState::eInputPressed; // key pressed
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
            mMouseState.mMouseMask[i] |= eInputState::eInputUp; // key up
        else
            mMouseState.mMouseMask[i] |= eInputState::eInputDown; // key down

        if (!previousButtonState && buttonState)
            mMouseState.mMouseMask[i] |= eInputState::eInputPressed; // key pressed
        else if (previousButtonState && buttonState)
            mMouseState.mMouseMask[i] |= eInputState::eInputReleased; // key released
    }

    mCurrentButtonState = newMouseState;

    mMouseMovement = glm::ivec2(x - mMouseState.mPos.x, y - mMouseState.mPos.y);
    mMouseState.mPos = glm::ivec2(x, y);

    mOnPostInputProcess.Broadcast(event_sdl);
    return event_sdl;
}

void InputManager::ResetToDefaultMappings()
{
    mMappings.clear();

    // Dev menu
    mMappings.insert({InputAction::DEV_MENU_KEY, SDL_SCANCODE_F1});

    // Default mappings
    mMappings.insert({InputAction::RETURN_KEY, SDL_SCANCODE_ESCAPE});

    // Player 1
    mMappings.insert({InputAction::P1_ENTER_KEY, SDL_SCANCODE_RETURN});
    mMappings.insert({InputAction::P1_MENU_UP_KEY, SDL_SCANCODE_W});
    mMappings.insert({InputAction::P1_MENU_LEFT_KEY, SDL_SCANCODE_A});
    mMappings.insert({InputAction::P1_MENU_DOWN_KEY, SDL_SCANCODE_S});
    mMappings.insert({InputAction::P1_MENU_RIGHT_KEY, SDL_SCANCODE_D});

    mMappings.insert({InputAction::P1_GAME_UP_KEY, SDL_SCANCODE_UP});
    mMappings.insert({InputAction::P1_GAME_LEFT_KEY, SDL_SCANCODE_LEFT});
    mMappings.insert({InputAction::P1_GAME_DOWN_KEY, SDL_SCANCODE_DOWN});
    mMappings.insert({InputAction::P1_GAME_RIGHT_KEY, SDL_SCANCODE_RIGHT});

    mMappings.insert({InputAction::P1_SCAN_CARD, SDL_SCANCODE_SPACE});
    mMappings.insert({InputAction::P1_NUMPAD_0, SDL_SCANCODE_KP_0});
    mMappings.insert({InputAction::P1_NUMPAD_1, SDL_SCANCODE_KP_1});
    mMappings.insert({InputAction::P1_NUMPAD_2, SDL_SCANCODE_KP_2});
    mMappings.insert({InputAction::P1_NUMPAD_3, SDL_SCANCODE_KP_3});
    mMappings.insert({InputAction::P1_NUMPAD_4, SDL_SCANCODE_KP_4});
    mMappings.insert({InputAction::P1_NUMPAD_5, SDL_SCANCODE_KP_5});
    mMappings.insert({InputAction::P1_NUMPAD_6, SDL_SCANCODE_KP_6});
    mMappings.insert({InputAction::P1_NUMPAD_7, SDL_SCANCODE_KP_7});
    mMappings.insert({InputAction::P1_NUMPAD_8, SDL_SCANCODE_KP_8});
    mMappings.insert({InputAction::P1_NUMPAD_9, SDL_SCANCODE_KP_9});

    // Player 2
    mMappings.insert({InputAction::P2_ENTER_KEY, SDL_SCANCODE_KP_ENTER});
    mMappings.insert({InputAction::P2_MENU_UP_KEY, SDL_SCANCODE_I});
    mMappings.insert({InputAction::P2_MENU_LEFT_KEY, SDL_SCANCODE_J});
    mMappings.insert({InputAction::P2_MENU_DOWN_KEY, SDL_SCANCODE_K});
    mMappings.insert({InputAction::P2_MENU_RIGHT_KEY, SDL_SCANCODE_L});

    mMappings.insert({InputAction::P2_GAME_UP_KEY, SDL_SCANCODE_T});
    mMappings.insert({InputAction::P2_GAME_LEFT_KEY, SDL_SCANCODE_F});
    mMappings.insert({InputAction::P2_GAME_DOWN_KEY, SDL_SCANCODE_G});
    mMappings.insert({InputAction::P2_GAME_RIGHT_KEY, SDL_SCANCODE_H});

    mMappings.insert({InputAction::P2_SCAN_CARD, SDL_SCANCODE_RCTRL});
    mMappings.insert({InputAction::P2_NUMPAD_0, SDL_SCANCODE_0});
    mMappings.insert({InputAction::P2_NUMPAD_1, SDL_SCANCODE_1});
    mMappings.insert({InputAction::P2_NUMPAD_2, SDL_SCANCODE_2});
    mMappings.insert({InputAction::P2_NUMPAD_3, SDL_SCANCODE_3});
    mMappings.insert({InputAction::P2_NUMPAD_4, SDL_SCANCODE_4});
    mMappings.insert({InputAction::P2_NUMPAD_5, SDL_SCANCODE_5});
    mMappings.insert({InputAction::P2_NUMPAD_6, SDL_SCANCODE_6});
    mMappings.insert({InputAction::P2_NUMPAD_7, SDL_SCANCODE_7});
    mMappings.insert({InputAction::P2_NUMPAD_8, SDL_SCANCODE_8});
    mMappings.insert({InputAction::P2_NUMPAD_9, SDL_SCANCODE_9});
}

bool InputManager::DoesMappingsJsonPathExist() { return std::filesystem::exists(MAPPINGS_JSON_PATH); }

void InputManager::LoadMappingsFromJson()
{
    if (!DoesMappingsJsonPathExist())
    {
        ResetToDefaultMappings();
        return;
    }

    std::ifstream file(MAPPINGS_JSON_PATH);
    if (!file.is_open())
    {
        PrintError("Failed to open {}", MAPPINGS_JSON_PATH);
        ResetToDefaultMappings();
        return;
    }

    json j;
    file >> j;
    file.close();

    mMappings.clear();
    for (auto& [actionStr, scancodeInt] : j.items())
    {
        const InputAction action = InputUtils::StringToInputAction(actionStr);
        const SDL_Scancode scancode = static_cast<SDL_Scancode>(scancodeInt.get<int>());
        mMappings.insert({action, scancode});
    }
}

void InputManager::SaveMappingsToJson()
{
    json j;
    for (auto& [action, scancode] : mMappings)
    {
        j[InputUtils::InputActionToString(action)] = static_cast<int>(scancode);
    }

    std::ofstream file("input_mappings.json");
    if (!file.is_open())
    {
        PrintError("Failed to open {} for writing", MAPPINGS_JSON_PATH);
        return;
    }

    file << j.dump(4);
    file.close();
}
