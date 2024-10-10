#pragma once
#include "Misc/Singleton.h"
#include "Phase.h"

#include <memory>
#include <string>
#include <map>

class PhaseManager
{
	Singleton(PhaseManager);

public:
	void Initialize();
	void Update(const float dt);

	void ChangeToScene(const std::string& phaseName);

private:
	template<typename T>
	void AddScene(const std::string& phaseName);

public:
	std::map<std::string, std::shared_ptr<Phase>> mPhaseList;
	std::shared_ptr<Phase> mCurrPhase;
};

#define PhaseMgr PhaseManager::Instance()