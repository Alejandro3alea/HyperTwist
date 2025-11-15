#include "InputUtils.h"
#include <boost/bimap.hpp>
#include <string>

namespace InputUtils
{
static const boost::bimap<InputAction, std::string> inputActionBimap = []
{
    boost::bimap<InputAction, std::string> m;

    // Dev Menu
    m.insert({InputAction::DEV_MENU_KEY, "DEV_MENU_KEY"});

    // Default
    m.insert({InputAction::RETURN_KEY, "RETURN_KEY"});

    // Player 1
    m.insert({InputAction::P1_ENTER_KEY, "P1_ENTER_KEY"});
    m.insert({InputAction::P1_MENU_UP_KEY, "P1_MENU_UP_KEY"});
    m.insert({InputAction::P1_MENU_LEFT_KEY, "P1_MENU_LEFT_KEY"});
    m.insert({InputAction::P1_MENU_DOWN_KEY, "P1_MENU_DOWN_KEY"});
    m.insert({InputAction::P1_MENU_RIGHT_KEY, "P1_MENU_RIGHT_KEY"});
    m.insert({InputAction::P1_GAME_UP_KEY, "P1_GAME_UP_KEY"});
    m.insert({InputAction::P1_GAME_LEFT_KEY, "P1_GAME_LEFT_KEY"});
    m.insert({InputAction::P1_GAME_DOWN_KEY, "P1_GAME_DOWN_KEY"});
    m.insert({InputAction::P1_GAME_RIGHT_KEY, "P1_GAME_RIGHT_KEY"});
    m.insert({InputAction::P1_SCAN_CARD, "P1_SCAN_CARD"});
    m.insert({InputAction::P1_NUMPAD_0, "P1_NUMPAD_0"});
    m.insert({InputAction::P1_NUMPAD_1, "P1_NUMPAD_1"});
    m.insert({InputAction::P1_NUMPAD_2, "P1_NUMPAD_2"});
    m.insert({InputAction::P1_NUMPAD_3, "P1_NUMPAD_3"});
    m.insert({InputAction::P1_NUMPAD_4, "P1_NUMPAD_4"});
    m.insert({InputAction::P1_NUMPAD_5, "P1_NUMPAD_5"});
    m.insert({InputAction::P1_NUMPAD_6, "P1_NUMPAD_6"});
    m.insert({InputAction::P1_NUMPAD_7, "P1_NUMPAD_7"});
    m.insert({InputAction::P1_NUMPAD_8, "P1_NUMPAD_8"});
    m.insert({InputAction::P1_NUMPAD_9, "P1_NUMPAD_9"});

    // Player 2
    m.insert({InputAction::P2_ENTER_KEY, "P2_ENTER_KEY"});
    m.insert({InputAction::P2_MENU_UP_KEY, "P2_MENU_UP_KEY"});
    m.insert({InputAction::P2_MENU_LEFT_KEY, "P2_MENU_LEFT_KEY"});
    m.insert({InputAction::P2_MENU_DOWN_KEY, "P2_MENU_DOWN_KEY"});
    m.insert({InputAction::P2_MENU_RIGHT_KEY, "P2_MENU_RIGHT_KEY"});
    m.insert({InputAction::P2_GAME_UP_KEY, "P2_GAME_UP_KEY"});
    m.insert({InputAction::P2_GAME_LEFT_KEY, "P2_GAME_LEFT_KEY"});
    m.insert({InputAction::P2_GAME_DOWN_KEY, "P2_GAME_DOWN_KEY"});
    m.insert({InputAction::P2_GAME_RIGHT_KEY, "P2_GAME_RIGHT_KEY"});
    m.insert({InputAction::P2_SCAN_CARD, "P2_SCAN_CARD"});
    m.insert({InputAction::P2_NUMPAD_0, "P2_NUMPAD_0"});
    m.insert({InputAction::P2_NUMPAD_1, "P2_NUMPAD_1"});
    m.insert({InputAction::P2_NUMPAD_2, "P2_NUMPAD_2"});
    m.insert({InputAction::P2_NUMPAD_3, "P2_NUMPAD_3"});
    m.insert({InputAction::P2_NUMPAD_4, "P2_NUMPAD_4"});
    m.insert({InputAction::P2_NUMPAD_5, "P2_NUMPAD_5"});
    m.insert({InputAction::P2_NUMPAD_6, "P2_NUMPAD_6"});
    m.insert({InputAction::P2_NUMPAD_7, "P2_NUMPAD_7"});
    m.insert({InputAction::P2_NUMPAD_8, "P2_NUMPAD_8"});
    m.insert({InputAction::P2_NUMPAD_9, "P2_NUMPAD_9"});

    return m;
}();

std::string InputActionToString(const InputAction& action)
{
    auto it = inputActionBimap.left.find(action);
    if (it != inputActionBimap.left.end())
        return it->second;
    return "UNKNOWN";
}

InputAction StringToInputAction(const std::string& str)
{
    auto it = inputActionBimap.right.find(str);
    if (it != inputActionBimap.right.end())
        return it->second;
    throw std::runtime_error("Unknown InputAction string: " + str);
}

} // namespace InputUtils
