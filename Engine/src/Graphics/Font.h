#pragma once
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <map>

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    uint32_t TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    uint32_t Advance;   // Horizontal offset to advance to next glyph
};


struct Font
{
	Font(const std::string& path);

    void BindFontTexture();

public:
    std::map<char, Character> mCharacters;
    unsigned mTextureArrayIndex;
};