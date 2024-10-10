#include "PhaseManager.h"
#include "Input/InputMgr.h"
#include "Graphics/GfxMgr.h"

// Phases
#include "DevMenu/DevMenuPhase.h"
#include "IntroScreen/IntroScreenPhase.h"
#include "AccountSelect/AccountSelectPhase.h"
#include "GameModeSelect/GameModeSelectPhase.h"
#include "SongSelect/SongSelectPhase.h"
#include "Gameplay/GameplayPhase.h"
#include "SongResults/SongResultsPhase.h"
#include "GameResults/GameResultsPhase.h"
#include "GameEnd/GameEndPhase.h"


PhaseManager* PhaseManager::mpInstance;

void PhaseManager::Initialize()
{
	AddScene<DevMenuPhase>("DevMenu");
	AddScene<IntroScreenPhase>("IntroScreen");
	AddScene<AccountSelectPhase>("AccountSelect");
	AddScene<GameModeSelectPhase>("GameModeSelect");
	AddScene<SongSelectPhase>("SongSelect");
	AddScene<GameplayPhase>("Gameplay");
	AddScene<SongResultsPhase>("SongResults");
	AddScene<GameResultsPhase>("GameResults");
	AddScene<GameEndPhase>("GameEnd");

	ChangeToScene("DevMenu");
}

void PhaseManager::Update(const float dt)
{
	if (InputMgr->isKeyPressed(SDL_SCANCODE_T))
	{
		if (mCurrPhase != mPhaseList["DevMenu"])
		{
			PhaseMgr->ChangeToScene("DevMenu");
		}
	}

	if (Phase* currPhase = mCurrPhase.get())
	{
		currPhase->OnTick(dt);
	}
}

void PhaseManager::ChangeToScene(const std::string& phaseName)
{
	if (Phase* oldPhase = mCurrPhase.get())
	{
		oldPhase->OnExit();
	}

	GfxMgr->CleanupRenderables();
	mCurrPhase = mPhaseList[phaseName];
	mCurrPhase->OnEnter();
}

template<typename T>
void PhaseManager::AddScene(const std::string& phaseName)
{
	mPhaseList[phaseName] = std::make_shared<T>();
}
