#pragma once
#include "GL/glew.h"
#include "glm/glm.hpp"

#include <string>

struct Texture
{
	Texture(const std::string& path);
	~Texture();

public:
	GLuint GetID() const { return mID; }
	const glm::ivec2 GetSize() const { return mSize; }
	const int GetColorChannels() const { return mColorChannels; }

	void Bind();

protected:
	GLuint mID;
	glm::ivec2 mSize;
	int mColorChannels;
};