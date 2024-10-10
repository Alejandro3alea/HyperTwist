#pragma once
#include "EngineApi.h"

struct GameVariables
{
	float ReceptorsOffset = 7.5f;
	float Inverse = -1.0f;

	float mZoom = 95.0f;
};

extern ENGINE_API GameVariables gGameVariables;