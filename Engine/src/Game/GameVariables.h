#pragma once
#include "EngineApi.h"

#include <cstdint>

struct GameVariables
{
	float ReceptorsOffset = 7.5f;
	float Inverse = -1.0f;

	float mZoom = 95.0f;

	uint16_t mMajorVersion = 0;
	uint16_t mMinorVersion = 1;
	uint16_t mPatchVersion = 0;
};

extern ENGINE_API GameVariables gGameVariables;