#pragma once
#include "ResourceMgr.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

struct Renderable
{
	Renderable();
	~Renderable();

	virtual void Render(Shader* shader = nullptr);
	Resource<Shader>* SetShader(const std::string& path);
	Resource<Texture>* SetTexture(const std::string& path);

public:
	Resource<Shader>* mShader = nullptr;
	Resource<Texture>* mTexture = nullptr;
	bool mbIsVisible = true;

	Transform transform;
	glm::vec4 mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};


struct TextureAtlas : public Renderable
{
	TextureAtlas();

	virtual void Render(Shader* shader = nullptr) override;

	void SetTextureScale(glm::uvec2 sizeInPixels);
	void SetTextureOffset(glm::uvec2 offsetInPixels);

	void SetTextureScale(const unsigned xPixels, unsigned yPixels) { SetTextureScale(glm::uvec2(xPixels, yPixels)); }
	void SetTextureOffset(const unsigned xPixels, unsigned yPixels) { SetTextureOffset(glm::uvec2(xPixels, yPixels)); }

public:
	glm::vec2 mTextureScale = { 1.0f, 1.0f };
	glm::vec2 mTextureOffset = { 0.0f, 0.0f };
};


struct LineList : public Renderable
{
	LineList(const std::vector<glm::vec2>& positions);
	~LineList();

	virtual void Render(Shader* shader = nullptr) override;

private:
	unsigned mVAO, mVBO;
	unsigned mLineCount = 0;
};
