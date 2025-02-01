#pragma once
#include "Composition/Transform.h"
#include "Graphics/Framebuffer.h"
#include "Resources/ResourceMgr.h"
#include "Graphics/Texture.h"

struct DancingBotBeatInfo
{
	float mMeasure = 0.0f;
	glm::vec3 mPos;
};

struct DancingBotFoot
{
	DancingBotFoot(const bool IsRightPos);

	void Update(const float currMeasure);

	void SetNextBeat(const float currMeasure);

	bool operator<(const DancingBotFoot& rhs) const { return mLastBeat.mMeasure < rhs.mLastBeat.mMeasure; }
	bool operator>(const DancingBotFoot& rhs) const { return mLastBeat.mMeasure > rhs.mLastBeat.mMeasure; }

public:
	Transform mTr;
	DancingBotBeatInfo mLastBeat, mNextBeat;
	DancingBotFoot* mOtherFoot = nullptr;
};

struct DancingBot
{
	DancingBot();

	void OnGui();

	void UpdateFeetPosition(const float currMeasure);
	void UpdateDancingTex();

public:
	DancingBotFoot mLeftFoot, mRightFoot;
	Framebuffer* mFb;

private:
	Resource<Texture>* mPadTex = nullptr;
	Resource<Texture>* mLFootTex = nullptr;
	Resource<Texture>* mRFootTex = nullptr;
};
