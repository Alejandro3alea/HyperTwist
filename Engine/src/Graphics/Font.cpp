#include "Font.h"
#include "Resources/ResourceLoadException.h"

#include <GL/glew.h>
#include <iostream>

Font::Font(const std::string& path)
{
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        throw ResourceLoadException(path, "FREETYPE: Could not init FreeType Library");
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face)) 
    {
        throw ResourceLoadException(path, "FREETYPE: Failed to load font");
    }
    else 
    {
        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 256, 256);

        // Disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &mTextureArrayIndex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrayIndex);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, 128, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

        // Load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0, 0, 0, int(c),
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows, 1,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            // Set texture options
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Store character for later use
            Character character = {
                uint32_t(c),
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<uint32_t>(face->glyph->advance.x)
            };
            mCharacters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    // Destroy FreeType since we don't need it for now
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void Font::BindFontTexture()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrayIndex);
}