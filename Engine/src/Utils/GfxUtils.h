#pragma once
#include "Misc/DataTypes.h"

#include <glm/glm.hpp>
#include <string>

namespace GfxUtils
{
    glm::vec3 HexToRGB(const std::string& hexCode);
    glm::vec4 HexToRGBA(const std::string& hexCode);

    std::string RGBToHex(const glm::vec3& color);
    std::string RGBAToHex(const glm::vec4& color);

    glm::vec3 IntToRGB(const u32 hexColor);
    glm::vec4 IntToRGBA(const u32 hexColor);

    glm::vec3 CharToRGB(const u8 r, const u8 g, const u8 b);
    glm::vec4 CharToRGBA(const u8 r, const u8 g, const u8 b, const u8 a);
}