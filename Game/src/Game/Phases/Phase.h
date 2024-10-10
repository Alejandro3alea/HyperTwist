#pragma once

struct Phase
{
	virtual void Initialize() {};

	virtual void OnEnter() {};
	virtual void OnTick(const float dt) = 0;
	virtual void OnExit() {};
};