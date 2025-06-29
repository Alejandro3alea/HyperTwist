#include "GfxUtils.h"

#include <stdexcept>
#include <algorithm>
#include <format>

namespace GfxUtils
{
    std::string FormatHexCode(const std::string& hexCode, const u8 codeSize)
    {
        // Remove beggining '#' if exists
        std::string result = hexCode;
        if (!result.empty() && result.front() == '#')
            result.erase(result.begin());

        // Support for one-digit digit format ('FAC' for RGB or 'C2F0' for RGBA)
        if (result.size() == codeSize / 2) {
            std::string expanded;
            expanded.reserve(codeSize);
            for (char c : result) {
                expanded.push_back(c);
                expanded.push_back(c);
            }
            result = std::move(expanded);
        }

        return result;
    }

    void CheckIfValidHexCode(const std::string& hexCode, const u8 codeSize)
    {
        if (hexCode.size() != codeSize)
            throw std::invalid_argument(std::format("Invalid hex length (expected {} or {} digits)", codeSize, codeSize / 2));

        const bool areAllHexDigits = std::all_of(hexCode.begin(), hexCode.end(), [](char c) {  return std::isxdigit(static_cast<unsigned char>(c)); });
        if (!areAllHexDigits)
            throw std::invalid_argument("Invalid characters in hex string");

    }

    glm::vec3 HexToRGB(const std::string &hexCode)
    {
        constexpr u8 codeSize = 6;
        const std::string hexStr = FormatHexCode(hexCode, codeSize);
        CheckIfValidHexCode(hexStr, codeSize);

        auto hexPairToUint = [&](i8 idx) {
            return std::stoul(hexStr.substr(idx, 2), nullptr, 16);
        };

        const float r = hexPairToUint(0) / 255.0f;
        const float g = hexPairToUint(2) / 255.0f;
        const float b = hexPairToUint(4) / 255.0f;

        return glm::vec3(r, g, b);
    }

    glm::vec4 HexToRGBA(const std::string &hexCode)
    {
        constexpr u8 codeSize = 8;
        const std::string hexStr = FormatHexCode(hexCode, codeSize);
        CheckIfValidHexCode(hexStr, codeSize);

        auto hexPairToUint = [&](i8 idx) {
            return std::stoul(hexStr.substr(idx, 2), nullptr, 16);
        };

        const float r = hexPairToUint(0) / 255.0f;
        const float g = hexPairToUint(2) / 255.0f;
        const float b = hexPairToUint(4) / 255.0f;
        const float a = hexPairToUint(6) / 255.0f;

        return glm::vec4(r, g, b, a);
    }

    std::string RGBToHex(const glm::vec3 &color)
    {
        auto toChar = [](f32 val) 
        {
            return static_cast<u8>(std::clamp(val * 255.0f + 0.5f, 0.0f, 255.0f));
        };

        u8 r = toChar(color.r);
        u8 g = toChar(color.g);
        u8 b = toChar(color.b);

        return std::format("#{:02X}{:02X}{:02X}", r, g, b);
    }

    std::string RGBAToHex(const glm::vec4 &color)
    {
        auto toChar = [](f32 val) 
        {
            return static_cast<u8>(std::clamp(val * 255.0f + 0.5f, 0.0f, 255.0f));
        };

        u8 r = toChar(color.r);
        u8 g = toChar(color.g);
        u8 b = toChar(color.b);
        u8 a = toChar(color.b);

        return std::format("#{:02X}{:02X}{:02X}{:02X}", r, g, b, a);
    }

    glm::vec3 IntToRGB(const u32 hexColor)
    {
        return glm::vec3(
            static_cast<float>((hexColor >> 16) & 0xFF) / 255.0f,
            static_cast<float>((hexColor >>  8) & 0xFF) / 255.0f,
            static_cast<float>( hexColor        & 0xFF) / 255.0f
        );
    }

    glm::vec4 IntToRGBA(const u32 hexColor)
    {
        return glm::vec4(
            static_cast<float>((hexColor >> 24) & 0xFF) / 255.0f,
            static_cast<float>((hexColor >> 16) & 0xFF) / 255.0f,
            static_cast<float>((hexColor >>  8) & 0xFF) / 255.0f,
            static_cast<float>( hexColor        & 0xFF) / 255.0f
        );
    }

    glm::vec3 CharToRGB(const u8 r, const u8 g, const u8 b)
    {
        return glm::vec3(
            static_cast<float>(r) / 255.0f, 
            static_cast<float>(g) / 255.0f, 
            static_cast<float>(b) / 255.0f
        );
    }

    glm::vec4 CharToRGBA(const u8 r, const u8 g, const u8 b, const u8 a)
    {
        return glm::vec4(
            static_cast<float>(r) / 255.0f, 
            static_cast<float>(g) / 255.0f, 
            static_cast<float>(b) / 255.0f, 
            static_cast<float>(a) / 255.0f
        );
    }
}