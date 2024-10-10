#include "RNG.h"

#include <time.h>

RandomNumberGenerator* RandomNumberGenerator::mpInstance;

void RandomNumberGenerator::Initialize()
{
	srand((unsigned)time(0));
}

int RandomNumberGenerator::GetValue(const int min, const int max)
{
	return min + std::rand() % (max - min);
}

bool RandomNumberGenerator::GetBool()
{
	return static_cast<bool>(std::rand() % 2);
}

float RandomNumberGenerator::GetValue(const float min, const float max)
{
	return min + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX / (max - min));
}

glm::vec2 RandomNumberGenerator::GetValue(const glm::vec2& min, const glm::vec2& max)
{
	glm::vec2 result;
	result.x = GetValue(min.x, max.x);
	result.y = GetValue(min.y, max.y);
	return result;
}

glm::vec3 RandomNumberGenerator::GetValue(const glm::vec3& min, const glm::vec3& max)
{
	glm::vec3 result;
	result.x = GetValue(min.x, max.x);
	result.y = GetValue(min.y, max.y);
	result.z = GetValue(min.z, max.z);
	return result;
}