#pragma once
#include "Misc/Singleton.h"

#include <glm.hpp>

#include <limits>

struct Vector3D;
class RandomNumberGenerator
{
	Singleton(RandomNumberGenerator)

public:
	void Initialize();

	bool GetBool();
	int GetValue(const int min = 0, const int max = INT_MAX);
	float GetValue(const float min = 0.0f, const float max = FLT_MAX);
	glm::vec2 GetValue(const glm::vec2& min, const glm::vec2& max);
	glm::vec3 GetValue(const glm::vec3& min, const glm::vec3& max);
};

#define RNG RandomNumberGenerator::Instance()
