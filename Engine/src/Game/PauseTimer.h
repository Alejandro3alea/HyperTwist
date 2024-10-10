#pragma once

struct PauseTimer
{
	PauseTimer(const float time = 0.0f);
	PauseTimer(const PauseTimer& rhs) = default;

	void Update();
	void Reset();

	bool IsActive() { return mTime > 0.0f; }

	float GetTime() const { return mTime; }
	void SetTime(const float time) { mTime = time; }

private:
	float mTime;
};