#include "Camera.h"
#include "WindowMgr.h"
#include "Game/GameVariables.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	mAspect = WindowMgr->mCurrentWindow->AspectRatio();
	mPos	= glm::vec3(0.f, 0.0f, 0.f);
}

glm::mat4x4 Camera::GetViewMtx()
{
	return glm::translate(glm::mat4(1.0f), -mPos);
}

glm::mat4x4 Camera::GetProjMtx()
{
	//const glm::uvec2 winSize = WindowMgr->mCurrentWindow->Size();
	const glm::uvec2 winSize = { 1920, 1080 };
	glm::vec2 viewRectHalf = { winSize.x / 1.0f, winSize.y / 1.0f };
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
	mPos.y = (gGameVariables.ReceptorsOffset + measure * 4.0f * mZoom) * gGameVariables.Inverse * -1.0f;
}
