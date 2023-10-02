#include "Camera.h"
#include "WindowMgr.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	mAspect = WindowMgr->mCurrentWindow->aspectRatio();
	mPos	= glm::vec3(0.f, 0.0f, 0.f);
}

glm::mat4x4 Camera::GetViewMtx()
{
	return glm::translate(glm::mat4(1.0f), -mPos);
}

glm::mat4x4 Camera::GetProjMtx()
{
	const float aspectRatio = WindowMgr->mCurrentWindow->aspectRatio();
	glm::vec2 viewRectHalf = { mZoom * aspectRatio / 2.0f,   mZoom / 2.0f };
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