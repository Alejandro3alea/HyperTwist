#include "GfxMgr.h"
#include "Shader.h"

#include "Graphics/Renderable.h"
#include "Resources/ResourceMgr.h"
#include "Camera.h"
#include "Editor.h"
#include "InputMgr.h"

#include <algorithm>

GraphicsManager* GraphicsManager::mpInstance;

void GraphicsManager::Initialize()
{
	// Initialize OpenGL library
 	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW Error: Failed to init" << std::endl;
		abort();
	}

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// enable depth buffering & back-face removal
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicsManager::Load()
{
	mQuadShader = ResourceMgr->Load<Shader>("data/engine/shaders/Quad.shader");
	mBGShader = ResourceMgr->Load<Shader>("data/engine/shaders/BG.shader");
	// @TODO: TODOTODOTODOTODOTODOTODOTODOTODOTODOTOOTODOO
	mLineShader = ResourceMgr->Load<Shader>("data/engine/shaders/Line.shader");

	// Load Quad
	mQuad = LoadQuad();

	mBackground = new Renderable();
	mBackground->mShader = mBGShader;
}

void GraphicsManager::Update()
{
	if (InputMgr->isKeyPressed(SDL_SCANCODE_F10))
		ResourceMgr->ReloadShaders();

	if (InputMgr->isButtonDown(InputMgr->eButtonRight))
	{
		glm::ivec2 mouseMovement = InputMgr->GetMouseMovement();
		Editor->mCam.Move({ 0.0f, mouseMovement.y * 0.05f });
	}
}

void GraphicsManager::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene(&Editor->mCam);
}

void GraphicsManager::Shutdown()
{
	glDeleteVertexArrays(1, &mQuad);
}

void GraphicsManager::SetBackground(Resource<Texture>* tex)
{
	mBackground->mTexture = tex;
}

void GraphicsManager::RenderScene(Camera* cam, Shader* shader)
{
	glm::mat4x4 view_cam = cam->GetViewMtx();
	glm::mat4x4 proj_cam = cam->GetProjMtx();
	glm::vec3 offset = cam->mPos;

	std::for_each(mRenderComps.begin(), mRenderComps.end(), [&offset, &proj_cam, &view_cam](Renderable* comp)
	{
		if (comp->mShader && comp->mbIsVisible)
		{
			Shader* shader = comp->mShader->get();
			shader->Bind();
			shader->SetUniform("uViewPos", offset);

			glm::mat4 model = comp->transform.GetModelMtx();
			shader->SetUniform("model", model);
			shader->SetUniform("view", view_cam);
			shader->SetUniform("proj", proj_cam);
			comp->Render();
		}
	});
}

GLuint GraphicsManager::LoadQuad() const
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