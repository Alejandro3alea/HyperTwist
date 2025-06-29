#include "Snappers.h"
#include "Game/GlobalVariables.h"
#include "Editor/Editor.h"
#include "ResourceMgr.h"

void Snappers::Initialize()
{
	LReceptor.mTexture = RReceptor.mTexture = ResourceMgr->LoadFromBasePath<Texture>("engine/texture/Beats.png");
	LReceptor.mShader = RReceptor.mShader = ResourceMgr->LoadFromBasePath<Shader>("engine/shaders/Receptors.shader");
	LReceptor.SetTextureScale({ 32, 32 });
	RReceptor.SetTextureScale({ 32, 32 });
	LReceptor.SetTextureOffset({ 32, 0 });;
	RReceptor.SetTextureOffset({ 32, 0 });
	LReceptor.transform.pos.x = -4.65f;
	RReceptor.transform.pos.x = 4.65f;
	LReceptor.transform.scale = RReceptor.transform.scale = glm::vec3(0.75f, 0.75f, 1.0f);
	
}

void Snappers::Update(const glm::vec3& camPos)
{
	float offset = gGlobalVariables.ReceptorsOffset * gGlobalVariables.Inverse;
	LReceptor.transform.pos.y = RReceptor.transform.pos.y = camPos.y + offset;
}

void Snappers::OnSnapMeasureChange(const bool isIncreased)
{
	if ( (isIncreased && mSnapMeasure == SnapMeasureType::eNote_32s) || (!isIncreased && mSnapMeasure == SnapMeasureType::eNote_NoSnap) )
		return;

	int increaseVal = isIncreased ? 1 : -1;
	mSnapMeasure += increaseVal;

	LReceptor.SetTextureOffset({mSnapMeasure * 32, 0});
	RReceptor.SetTextureOffset({ mSnapMeasure * 32, 0 });
}

float Snappers::GetPrevMeasure(const float currMeasure)
{
	const float measures[] = {
		4.0f,
		1.0f,
		1.0f / 2.0f,
		1.0f / 3.0f,
		1.0f / 4.0f,
		1.0f / 6.0f,
		1.0f / 8.0f,
		1.0f / 10.0f
	};

	float increment = measures[mSnapMeasure];
	float currVal = std::ceilf(currMeasure);
	while (currVal >= currMeasure)
		currVal -= increment;

	return currVal;
}

float Snappers::GetNextMeasure(const float currMeasure)
{
	const float measures[] = {
		4.0f,
		1.0f,
		1.0f / 2.0f,
		1.0f / 3.0f,
		1.0f / 4.0f,
		1.0f / 6.0f,
		1.0f / 8.0f,
		1.0f / 10.0f
	};

	float increment = measures[mSnapMeasure];
	float currVal = std::floorf(currMeasure);
	while (currVal <= currMeasure)
		currVal += increment;

	return currVal;
}