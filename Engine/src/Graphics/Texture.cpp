#include "Texture.h"
#include "Resources/ResourceMgr.h"

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include <iostream>

Texture::Texture(const std::string& path)
{
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &mSize.x, &mSize.y, &mColorChannels, 4);
	if (!data)
	{
		const std::string reason = "Error loading texture " + path + ":\n" + stbi_failure_reason() + '\n';
		stbi_image_free(data);
		throw ResourceLoadException(path, reason);
	}

	GLenum eColorChannel = (mColorChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSize.x, mSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

Texture::~Texture()
{
	std::cout << "[DEBUG] Deleted texture with ID " << mID << std::endl;
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, mID);
}
