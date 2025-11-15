#pragma once
#include "Input/InputAction.h"

#include <string>

namespace InputUtils
{
std::string InputActionToString(const InputAction& action);
InputAction StringToInputAction(const std::string& str);
} // namespace InputUtils