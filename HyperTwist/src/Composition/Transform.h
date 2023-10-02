#pragma once
#include <glm/glm.hpp>

struct Transform
{
	Transform() : pos(glm::vec3(0.0f)), scale(glm::vec3(1.0f)), rotation(0.0f) {}
	Transform(const glm::vec2& _pos, const glm::vec2& _scale, const float _rotation) : pos(glm::vec3(_pos.x, _pos.y, 0.0f)), scale(glm::vec3(_scale.x, _scale.y, 1.0f)), rotation(_rotation) {}
	Transform(const glm::vec3& _pos, const glm::vec2& _scale, const float _rotation) : pos(_pos), scale(glm::vec3(_scale.x, _scale.y, 1.0f)), rotation(_rotation) {}

	glm::vec3 pos;
	glm::vec3 scale;
	float rotation;

public:
	glm::mat4 GetModelMtx();

	Transform& Concatenate(const Transform& rhs);
	Transform& InvConcatenate(const Transform& rhs);

	const Transform operator+(const Transform& rhs);
	const Transform operator-(const Transform& rhs);
};