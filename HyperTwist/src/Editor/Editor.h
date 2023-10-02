#pragma once
#include "Singleton.h"
#include "ResourceMgr.h"
#include "Texture.h"
#include "Camera.h"
#include "Game/Song.h"

#include "Game/UI/DDR/Receptors.h"
#include "Game/UI/DDR/MeasureLines.h"

#include "imgui/imgui.h"

#include <map>

struct SceneNode;

class ImGuiEditor
{
	Singleton(ImGuiEditor);

	friend struct SceneNode;
	friend struct Component;

public:
	void Initialize();
	void Load();
	void Update(const float dt);
	void Shutdown();

	void Dockspace();
	void MainMenu();

	// HyperTwist
	void StartSong();

	void SongPropertiesWindow();
	void ChartListWindow();
	void ChartPropertiesWindow();
	void DancingBotWindow();

	void NewChartPopup();

	void ChangeDifficulty(const ChartDifficulty Difficulty);
	ChartDifficulty GetCurrentDifficulty() { return mCurrDifficulty; }

private:
	void LoadIcon(const std::string& name);

public:
	bool mbDockWindowOpen;
	ImGuiDockNodeFlags mDockspaceFlags = ImGuiDockNodeFlags_None;

	SceneNode* mSelectedNode = nullptr;

	unsigned long selectionMask = 0;

	Camera mCam;

	// HyperTwist
	Resource<Song>* mSong = nullptr;
	Resource<Audio>* mBeatTick = nullptr;
	Resource<Audio>* mNoteTick = nullptr;

private:
	std::map<std::string, Resource<Texture>*> mIcons;

	// HyperTwist
	float mCurrMeasure = 0.0f;
	float mNextBeat = 0.0f;
	float mOffsetTime = 0.0f;
	Receptors mReceptors;
	MeasureLines mMeasureLines;

	ChartDifficulty mCurrDifficulty;
};

#define Editor ImGuiEditor::Instance()