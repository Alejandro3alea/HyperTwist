#pragma once
#include "Graphics/Renderable.h"

enum SnapMeasureType
{
	eNote_NoSnap,
	eNote_4s,	// Reds
	eNote_8s,	// Blues
	eNote_12s,	// Greens
	eNote_16s,	// Yellows
	eNote_24s,	// Purples
	eNote_32s	// Oranges
};

struct Snappers
{
	void Initialize();
	void Update(const glm::vec3& camPos);

	void OnSnapMeasureChange(const bool isIncreased);

	float GetPrevMeasure(const float currMeasure);
	float GetNextMeasure(const float currMeasure);

	
private:
	unsigned mSnapMeasure = SnapMeasureType::eNote_4s;

	TextureAtlas LReceptor;
	TextureAtlas RReceptor;
};