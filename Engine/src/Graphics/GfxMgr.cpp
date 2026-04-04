#include "GfxMgr.h"
#include "Shader.h"

#include "Camera.h"
#include "Graphics/Renderables/Renderable.h"
#include "Input/InputMgr.h"
#include "Misc/Timer.h"
#include "Resources/ResourceMgr.h"
#include "WindowMgr.h"

#include <algorithm>

GraphicsManager* GraphicsManager::mpInstance;

void GraphicsManager::Initialize()
{
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) // Ensure causes errors
    {
        PrintError("Failed to initialize GLAD!");
        abort();
    }

    std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (version.empty())
        PrintWarning("GLEW Error: Failed to init");
    else
        PrintDebug("OpenGL Version: " + version + "\n");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    InputMgr->mOnWindowResize.Add([](const uint32_t width, const uint32_t height) { glViewport(0, 0, width, height); });

    LoadMainResources();
}

void GraphicsManager::LoadMainResources()
{
    mQuadShader = ResourceMgr->Load<Shader>("engine/shaders/Quad.shader");
    mBGShader = ResourceMgr->Load<Shader>("engine/shaders/BG.shader");
    mLineShader = ResourceMgr->Load<Shader>("engine/shaders/Line.shader");

    // Load Quad
    mQuad = CreateQuadModel();

    mBackground = new Renderable();
    mBackground->mShader = mBGShader;
    mBackground->mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void GraphicsManager::Render()
{
    if (InputMgr->isKeyPressed(SDL_SCANCODE_F10))
        ResourceMgr->ReloadShaders();

    mOnPreRender.Broadcast();

    glClearColor(1.0f, 0.2f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mCurrRenderQueue)
        mCurrRenderQueue->Execute();
    else
        RenderScene(&mCam);

    mOnPostRender.Broadcast();

    mTime += TimeMgr->deltaTime;
}

void GraphicsManager::Shutdown() { glDeleteVertexArrays(1, &mQuad); }

void GraphicsManager::ShowBackground() { mBackground->mbIsVisible = true; }

void GraphicsManager::HideBackground() { mBackground->mbIsVisible = false; }

void GraphicsManager::SetBackgroundTexture(Resource<Texture>* tex) { mBackground->mTexture = tex; }

void GraphicsManager::SetBackgroundShader(Resource<Shader>* shader) { mBackground->mShader = shader; }

Resource<Texture>* GraphicsManager::GetBackgroundTexture() { return mBackground->mTexture; }

Resource<Shader>* GraphicsManager::GetBackgroundShader() { return mBackground->mShader; }

GLuint GraphicsManager::CreateQuadModel() const
{
    const float vertices[] = {
        // positions    // texture coords
        1.0f,  1.0f,  1.0f, 1.0f, // top right
        1.0f,  -1.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, 1.0f  // top left
    };
    const unsigned int indices[] = {
        3, 1, 0, // first triangle
        3, 2, 1  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return VAO;
}

void GraphicsManager::CleanupRenderables()
{
    Resource<Texture>* bgTexture = mBackground->mTexture;
    bool bgIsVisible = mBackground->mbIsVisible;

    Transform bgTransform = mBackground->transform;
    glm::vec4 bgColor = mBackground->mColor;
    mRenderComps.clear();

    mBackground = new Renderable();
    mBackground->mShader = mBGShader;
    mBackground->mTexture = bgTexture;
    mBackground->mbIsVisible = bgIsVisible;
    mBackground->transform = bgTransform;
    mBackground->mColor = bgColor;
}

void GraphicsManager::RenderScene(Camera* cam, Shader* shader)
{
    Camera* using_cam = cam ? cam : &mCam;
    glm::mat4x4 view_cam = using_cam->GetViewMtx();
    glm::mat4x4 proj_cam = using_cam->GetProjMtx();
    glm::vec3 offset = using_cam->mPos;

    std::sort(mRenderComps.begin(), mRenderComps.end(),
              [](IRenderable* lhs, IRenderable* rhs) { return lhs->transform.pos.z < rhs->transform.pos.z; });

    WindowMgr->SetWindowViewport();

    if (shader)
    {
        shader->Bind();
        shader->SetUniform("uViewPos", offset);
        shader->SetUniform("uTime", mTime);
        shader->SetUniform("view", view_cam);
        shader->SetUniform("proj", proj_cam);

        std::for_each(mRenderComps.begin(), mRenderComps.end(),
                      [&shader](IRenderable* comp)
                      {
                          if (comp->mbIsVisible)
                          {
                              const Transform t = comp->mParentTransform.has_value()
                                                      ? comp->transform + comp->mParentTransform.value()
                                                      : comp->transform;
                              glm::mat4 model = t.GetModelMtx();
                              shader->SetUniform("model", model);
                              comp->Render();
                          }
                      });
    }
    else
    {
        std::for_each(mRenderComps.begin(), mRenderComps.end(),
                      [this, &shader, &offset, &proj_cam, &view_cam](IRenderable* comp)
                      {
                          if (comp->mShader && comp->mbIsVisible)
                          {
                              Shader* currShader = comp->mShader->get();
                              currShader->Bind();
                              currShader->SetUniform("uViewPos", offset);
                              currShader->SetUniform("uTime", mTime);

                              const Transform t = comp->mParentTransform.has_value()
                                                      ? comp->transform + comp->mParentTransform.value()
                                                      : comp->transform;
                              glm::mat4 model = t.GetModelMtx();
                              currShader->SetUniform("model", model);
                              currShader->SetUniform("view", view_cam);
                              currShader->SetUniform("proj", proj_cam);
                              comp->Render();
                          }
                      });
    }
}