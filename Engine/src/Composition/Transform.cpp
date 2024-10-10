#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

glm::mat4 Transform::GetModelMtx() const
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale);
	return model;
}

Transform& Transform::Concatenate(const Transform& rhs)
{
	scale *= rhs.scale;
	rotation += rhs.rotation;
	pos = rhs.rotation * (rhs.scale * pos) + rhs.pos;

	return *this;
}

Transform& Transform::InvConcatenate(const Transform& rhs)
{
	scale /= rhs.scale;
	rotation -= rhs.rotation;
	pos = (1.0f / rhs.scale) * (-rhs.rotation * (pos - rhs.pos));

	return *this;
}

const Transform Transform::operator+(const Transform& rhs)
{
	return Concatenate(rhs);
}

const Transform Transform::operator-(const Transform& rhs)
{
	return InvConcatenate(rhs);
}
