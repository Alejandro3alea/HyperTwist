#pragma once
#include "Camera.h"
#include "Misc/Singleton.h"
#include "Resources/ResourceMgr.h"
#include "Composition/Events/Event.h"

#include <GL/glew.h>

#include <vector>
#include <string>
#include <map>

struct Renderable;
struct Texture;
struct Camera;
struct Shader;

CREATE_EVENT(OnPreRender);
CREATE_EVENT(OnPostRender);

class GraphicsManager
{
	Singleton(GraphicsManager);

public:
	void Initialize();
	void LoadMainResources();
	void Render();
	void Shutdown();

	GLuint GetQuad() { return mQuad; }

	Shader* GetQuadShader() { return mQuadShader->get(); }
	Shader* GetBGShader()	  { return mBGShader->get();   }
	Shader* GetLineShader() { return mLineShader->get(); }

	void SetBackgroundTexture(Resource<Texture>* tex);
	void SetBackgroundShader(Resource<Shader>* shader);
	Resource<Texture>* GetBackgroundTexture();
	Resource<Shader>* GetBackgroundShader();

	GLuint CreateQuadModel() const;

	void CleanupRenderables();

private:
	void RenderScene(Camera* cam, Shader* shader = nullptr);

public:
	Camera mCam;
	std::vector<Renderable*> mRenderComps;

	OnPreRender mOnPreRender;
	OnPostRender mOnPostRender;

private:
	Resource<Shader>* mQuadShader;
	Resource<Shader>* mBGShader;
	Resource<Shader>* mLineShader;
	GLuint mQuad;

	float mTime = 0.0f;

	Renderable* mBackground;
};

#define GfxMgr GraphicsManager::Instance()