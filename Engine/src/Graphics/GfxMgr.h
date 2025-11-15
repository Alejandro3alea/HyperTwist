#pragma once
#include "Camera.h"
#include "Composition/Events/Event.h"
#include "Misc/Singleton.h"
#include "Resources/ResourceMgr.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <map>
#include <string>
#include <vector>

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
    Shader* GetBGShader() { return mBGShader->get(); }
    Shader* GetLineShader() { return mLineShader->get(); }

    void ShowBackground();
    void HideBackground();

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

    // @TODO: Enable/Disable MSAA
    // bool mIsMSAAEnabled = true;

  private:
    Resource<Shader>* mQuadShader;
    Resource<Shader>* mBGShader;
    Resource<Shader>* mLineShader;
    GLuint mQuad;

    float mTime = 0.0f;

    Renderable* mBackground;
};

#define GfxMgr GraphicsManager::Instance()