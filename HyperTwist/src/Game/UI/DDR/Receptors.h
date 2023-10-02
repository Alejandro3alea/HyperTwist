#pragma once
#include "Graphics/Renderable.h"

struct Receptors
{
	void Initialize();
	void Update();

	TextureAtlas mReceptorOn[4];
	TextureAtlas mReceptorOff[4];
};