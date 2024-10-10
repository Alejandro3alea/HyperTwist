#pragma once
#include "Misc/Singleton.h"
#include "Graphics/Texture.h"
#include "Graphics/Camera.h"
#include "Composition/Events/Event.h"

#include "Game/Song.h"
#include "Game/PauseTimer.h"
 
#include "Game/UI/DDR/Receptors.h"
#include "Game/UI/DDR/Snappers.h"
#include "Game/UI/DDR/MeasureLines.h"
#include "Game/UI/DDR/DancingBot.h"

#include "imgui/imgui.h"

#include <map>


class ImGuiEditor
{
	Singleton(ImGuiEditor);

	friend class GraphicsManager;
	friend struct SceneNode;
	friend struct Component;

public:
	void Initialize();
	void Load();
	void Update(const double dt);
	void Shutdown();

	void Dockspace();
	void MainMenu();

	void SetImGuiStyle();

	// HyperTwist
	void StartSong();

	void SongPropertiesWindow();
	void ChartListWindow();
	void ChartPropertiesWindow();
	void AdjustSyncWindow();

	void NewChartPopup();

	void ChangeDifficulty(const ChartDifficulty Difficulty);
	ChartDifficulty GetCurrentDifficulty() { return mCurrDifficulty; }

	Chart* GetCurrChart();
	std::multiset<Note*, NoteCompare>& GetCurrNotes() { return mSong->get()->mCharts[mCurrDifficulty]->mNotes; }
	Note* GetNextNote(const double currMeasure);
	double GetNextStop(const double currMeasure);

	double GetNextStep(const double dt);

	void CreateOrDeleteNote(Note* newNote);

private:
	void LoadIcon(const std::string& name);

	bool HandlePauseTimers(const double dt);

	void HandleEditorInput();

public:
	bool mbDockWindowOpen;
	ImGuiDockNodeFlags mDockspaceFlags = ImGuiDockNodeFlags_None;

	unsigned long selectionMask = 0;

	// HyperTwist
	Resource<Song>* mSong = nullptr;
	Resource<Audio>* mBeatTick = nullptr;
	Resource<Audio>* mNoteTick = nullptr;

	PauseTimer mStartTimer;
	PauseTimer mPauseTimer;

private:
	std::map<std::string, Resource<Texture>*> mIcons;

	// HyperTwist
	double mCurrMeasure = 0.0;
	float mTimePlayedInSeconds = 0.0f;  // Used for songs offsets @TODO CHANGE THIS
	float mNextBeat = 0.0f;
	Receptors mReceptors;
	Snappers mSnappers;
	MeasureLines mMeasureLines;

	ChartDifficulty mCurrDifficulty;

	std::unique_ptr<DancingBot> mDancingBotWindow;
};

#define Editor ImGuiEditor::Instance()