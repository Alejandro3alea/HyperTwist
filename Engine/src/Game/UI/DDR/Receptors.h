#pragma once
#include "Graphics/Renderable.h"

struct Receptors
{
	void Initialize();
	void Update(const glm::vec3& camPos);

	TextureAtlas mReceptorOn[4];
	TextureAtlas mReceptorOff[4];
};