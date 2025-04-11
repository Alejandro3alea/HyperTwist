#pragma once
#include "Resources/ResourceMgr.h"
#include "Composition/Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "Font.h"

#include <array>

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct Renderable
{
	Renderable();
	~Renderable();

	virtual void Render(Shader* shader = nullptr);
	Resource<Shader>* SetShader(const std::string& path);
	Resource<Shader>* SetShader(Resource<Shader>* shader);
	Resource<Texture>* SetTexture(const std::string& path);
	Resource<Texture>* SetTexture(Resource<Texture>* texture);

public:
	Resource<Shader>* mShader = nullptr;
	Resource<Texture>* mTexture = nullptr;
	bool mbIsVisible = true;

	Transform transform;
	glm::vec4 mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct TextureAtlas : public Renderable
{
	TextureAtlas();

	virtual void Render(Shader* shader = nullptr) override;

	glm::vec2 SetTextureScale(glm::uvec2 sizeInPixels);
	glm::vec2 SetTextureOffset(glm::uvec2 offsetInPixels);
	glm::vec2 SetTextureScale(const unsigned xPixels, unsigned yPixels) { return SetTextureScale(glm::uvec2(xPixels, yPixels)); }
	glm::vec2 SetTextureOffset(const unsigned xPixels, unsigned yPixels) { return SetTextureOffset(glm::uvec2(xPixels, yPixels)); }

public:
	glm::vec2 mTextureScale;
	glm::vec2 mTextureOffset;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct LineList : public Renderable
{
	LineList(const std::vector<glm::vec2>& positions);
	~LineList();

	virtual void Render(Shader* shader = nullptr) override;

private:
	unsigned mVAO, mVBO;
	unsigned mLineCount = 0;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct Chart;

#define MAX_NOTES 10000

struct NoteRenderer : public TextureAtlas
{
	NoteRenderer(Chart* inChart);

	virtual void Render(Shader* shader = nullptr) override;

	void UpdateParams();
	void UpdateVBOs();

protected:
	Chart* mChart = nullptr;

	GLuint mVAO;
	GLuint mFloatVBO; // All float/vec variables stored in a VBO

	std::array<glm::vec2, MAX_NOTES * 3> mFloatParams;

	std::vector<glm::mat4> mRotations;
	std::vector<float> mXPositions;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct HoldNoteBodyRenderer : public TextureAtlas
{
	HoldNoteBodyRenderer(Chart* inChart);

	virtual void Render(Shader* shader = nullptr) override final;

	void UpdateParams();
	void UpdateVBOs();

private:
	Chart* mChart = nullptr;

	GLuint mVAO;
	GLuint mFloatVBO; // All float/vec variables stored in a VBO

	std::array<float, MAX_NOTES * 4> mFloatParams;

	Resource<Texture>* mRollNoteTexture = nullptr;
	Resource<Texture>* mHoldBottomCapTexture = nullptr;
	Resource<Texture>* mRollBottomCapTexture = nullptr;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

struct MineRenderer : public TextureAtlas
{
	MineRenderer(Chart* inChart);

	virtual void Render(Shader* shader = nullptr) override final;

	void UpdateParams();
	void UpdateVBOs();

private:
	Chart* mChart = nullptr;

	GLuint mVAO;
	GLuint mFloatVBO; // All float/vec variables stored in a VBO

	std::array<glm::vec2, MAX_NOTES * 3> mFloatParams;
	std::vector<float> mXPositions;
};

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

#define FONTRENDERER_TEXTDEFAULTSIZE 0.25f
#define FONTRENDERER_STRING_LIMIT 255
struct FontRenderer : public Renderable
{
	FontRenderer(const std::string& text = "", Resource<Font>* font = ResourceMgr->Load<Font>("engine/fonts/Roboto-Medium.ttf"));

	virtual void Render(Shader* shader = nullptr) override;
	Resource<Font>* SetFont(const std::string& path);
	void SetText(const std::string& text);

private:
	void InitializeVertexData();

	void TextRenderCall(int length, GLuint shaderID);

	glm::uvec2 GetTextSize();

private:
	Resource<Font>* mFont = nullptr;
	std::string mText;

	GLuint mVAO;
	GLuint mVBO;

	std::array<glm::mat4, FONTRENDERER_STRING_LIMIT> mTransforms;
	std::array<int, FONTRENDERER_STRING_LIMIT> mLetterMap;
};