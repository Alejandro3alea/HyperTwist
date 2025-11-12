#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>

struct Texture
{
    explicit Texture(const std::string& path);
    ~Texture();

  public:
    [[nodiscard]] GLuint GetID() const { return mID; }
    [[nodiscard]] glm::ivec2 GetSize() const { return mSize; }
    [[nodiscard]] int GetColorChannels() const { return mColorChannels; }

    void Bind();

  protected:
    GLuint mID = 0;
    glm::ivec2 mSize = {};
    int mColorChannels = 0;
};