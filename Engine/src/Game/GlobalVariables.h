#pragma once
#include "EngineApi.h"
#include "Misc/DataTypes.h"

#include <cstdint>

struct GlobalVariables
{
	// @TODO: Editor variable
	f32 ReceptorsOffset = 7.5f;
	// @TODO: Editor variable
	f32 Inverse = -1.0f;

	// @TODO: Editor variable
	f32 mZoom = 95.0f;


	u16 mMajorVersion = 0;
	u16 mMinorVersion = 0;
	u16 mPatchVersion = 0;
};

extern ENGINE_API GlobalVariables gGlobalVariables;