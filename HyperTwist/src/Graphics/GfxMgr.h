#pragma once
#include "Misc/Singleton.h"
#include "ResourceMgr.h"

#include <GL/glew.h>

#include <vector>
#include <string>
#include <map>

struct Renderable;
struct Texture;
struct Camera;
struct Shader;

class GraphicsManager
{
	Singleton(GraphicsManager);

public:
	void Initialize();
	void Load();
	void Update();
	void Render();
	void Shutdown();

	GLuint GetQuad() { return mQuad; }

	void SetBackground(Resource<Texture>* tex);

private:
	void RenderScene(Camera* cam, Shader* shader = nullptr);

	GLuint LoadQuad() const;

public:
	std::vector<Renderable*> mRenderComps;

private:
	Resource<Shader>* mQuadShader;
	Resource<Shader>* mBGShader;
	Resource<Shader>* mLineShader;
	GLuint mQuad;

	Renderable* mBackground;
};

#define GfxMgr GraphicsManager::Instance()