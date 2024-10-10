#pragma once
#include <glm/glm.hpp>

namespace Color
{
	inline glm::vec4 FromInt(const uint8_t r = 255, const uint8_t g = 255, const uint8_t b = 255, const uint8_t a = 255)
	{
		return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
}