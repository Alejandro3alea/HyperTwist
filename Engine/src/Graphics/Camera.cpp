#include "Camera.h"
#include "WindowMgr.h"
#include "Game/GlobalVariables.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	mPos	= glm::vec3(0.f, 0.0f, 0.f);
}

glm::mat4x4 Camera::GetViewMtx()
{
	return glm::translate(glm::mat4(1.0f), -mPos);
}

glm::mat4x4 Camera::GetProjMtx()
{
	const glm::uvec2 winHalf = { 960, 540 }; // { 1920, 1080 } / 2.0f
	glm::vec2 viewRectHalf = { winHalf.x, winHalf.y };
	return glm::ortho(-viewRectHalf.x, viewRectHalf.x, -viewRectHalf.y, viewRectHalf.y, -100.0f, 1000.0f);
}

void Camera::Move(const glm::vec2& vec)
{
	mPos += glm::vec3(vec.x, vec.y, 0.0f);
}

void Camera::Move(const glm::vec3& vec)
{
	mPos += vec;
}

void Camera::SetPositionFromMeasure(const float measure)
{
	mPos.y = (gGlobalVariables.ReceptorsOffset + measure * 4.0f * mZoom) * gGlobalVariables.Inverse * -1.0f;
}
