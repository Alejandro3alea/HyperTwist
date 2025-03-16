#pragma once

struct Phase
{
	virtual ~Phase() = default;
	virtual void Initialize() {};

	virtual void OnEnter() {};
	virtual void OnTick(float dt) = 0;
	virtual void OnExit() {};
};