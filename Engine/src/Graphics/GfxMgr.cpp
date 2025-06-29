#include "GfxMgr.h"
#include "Shader.h"

#include "Graphics/Renderable.h"
#include "Resources/ResourceMgr.h"
#include "Input/InputMgr.h"
#include "Misc/Timer.h"
#include "Camera.h"

#include <algorithm>

GraphicsManager* GraphicsManager::mpInstance;

void GraphicsManager::Initialize()
{
	// Initialize OpenGL library
 	glewExperimental = true;
	if (glewInit() != GLEW_OK) // Ensure causes errors
	{
		std::cout << "GLEW Error: Failed to init" << std::endl;
		abort();
	}
	std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	if (version.empty()) 
		std::cerr << "Unable to retrieve OpenGL version." << std::endl;
	else
		std::cout << "OpenGL Version: " << version << std::endl << std::endl;

	glClearColor(1.0f, 0.2f, 1.0f, 1.0f);

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
}

void GraphicsManager::Render()
{
	if (InputMgr->isKeyPressed(SDL_SCANCODE_F10))
		ResourceMgr->ReloadShaders();

	mOnPreRender.Broadcast();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderScene(&mCam);
	mOnPostRender.Broadcast();

	mTime += TimeMgr->deltaTime;
}

void GraphicsManager::Shutdown()
{
	glDeleteVertexArrays(1, &mQuad);
}

void GraphicsManager::SetBackgroundTexture(Resource<Texture>* tex)
{
	mBackground->mTexture = tex;
}

void GraphicsManager::SetBackgroundShader(Resource<Shader>* shader)
{
	mBackground->mShader = shader;
}

Resource<Texture>* GraphicsManager::GetBackgroundTexture()
{
	return mBackground->mTexture;
}

Resource<Shader>* GraphicsManager::GetBackgroundShader()
{
	return mBackground->mShader;
}

GLuint GraphicsManager::CreateQuadModel() const
{
	const float vertices[] = {
		// positions    // texture coords
		 1.0f,  1.0f,    1.0f, 1.0f, // top right
		 1.0f, -1.0f,    1.0f, 0.0f, // bottom right
		-1.0f, -1.0f,    0.0f, 0.0f, // bottom left
		-1.0f,  1.0f,    0.0f, 1.0f  // top left 
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
	Renderable* bgRenderable = mRenderComps[0];
	Resource<Texture>* bgTexture = bgRenderable->mTexture;
	bool bgIsVisible = bgRenderable->mbIsVisible;

	Transform bgTransform = bgRenderable->transform;
	glm::vec4 bgColor = bgRenderable->mColor;
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
	glm::mat4x4 view_cam = cam->GetViewMtx();
	glm::mat4x4 proj_cam = cam->GetProjMtx();
	glm::vec3 offset = cam->mPos;

	std::sort(mRenderComps.begin(), mRenderComps.end(), [](Renderable* lhs, Renderable* rhs)
		{
			return lhs->transform.pos.z < rhs->transform.pos.z;
		});

	std::for_each(mRenderComps.begin(), mRenderComps.end(), [this, &offset, &proj_cam, &view_cam](Renderable* comp)
		{
			if (comp->mShader && comp->mbIsVisible)
			{
				Shader* shader = comp->mShader->get();
				shader->Bind();
				shader->SetUniform("uViewPos", offset);
				shader->SetUniform("uTime", mTime);

    			const Transform t = comp->mParentTransform.has_value() ? comp->transform + comp->mParentTransform.value() : comp->transform;
				glm::mat4 model = t.GetModelMtx();
				shader->SetUniform("model", model);
				shader->SetUniform("view", view_cam);
				shader->SetUniform("proj", proj_cam);
				comp->Render();
			}
		});
}