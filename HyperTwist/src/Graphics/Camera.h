#pragma once
#include <glm.hpp>

struct Camera
{
	Camera();

	glm::mat4x4 GetViewMtx();
	glm::mat4x4 GetProjMtx();

	void Move(const glm::vec2& vec);
	void Move(const glm::vec3& vec);

public:
	float mAspect;
	glm::vec3 mPos;
	float mZoom = 25.0f;
};