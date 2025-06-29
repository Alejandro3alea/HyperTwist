#include "Editor.h"
#include "Graphics/WindowMgr.h"
#include "Graphics/Renderable.h"
#include "Input/InputMgr.h"
#include "Tools/Profiler/Profiler.h"
#include "Game/GameVariables.h"
#include "Game/GlobalEvents.h"
#include "Graphics/GfxMgr.h"
#include "ImGuiWidgets.h"
#include "Misc/Timer.h"

#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "aubio/aubio.h"

#include <Windows.h>

ImGuiEditor* ImGuiEditor::mpInstance;

void ImGuiEditor::Initialize()
{
	const char* glsl_version = "#version 330";

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		  // Enable docking (See ImGui Docking tutorial)

	mDockspaceFlags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
	mDockspaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;

	SetImGuiStyle();

	Window* win = WindowMgr->mCurrentWindow.get();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(*win, win);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// HyperTwist
	mBeatTick = ResourceMgr->LoadFromBasePath<Audio>("engine/audio/sfx/BeatTick.wav");
	mNoteTick = ResourceMgr->LoadFromBasePath<Audio>("engine/audio/sfx/NoteTick.wav");
	mSnappers.Initialize();
	mDancingBotWindow = std::unique_ptr<DancingBot>(new DancingBot());

	Profiler->Initialize();

	GfxMgr->mOnPreRender.Add([this]() { HandleEditorInput(); });

	GlobalEvents::gOnChartEdit.Add([](Chart* chart) { chart->mNoteRenderer->UpdateVBOs(); });
	GlobalEvents::gOnChartEdit.Add([](Chart* chart) { chart->mHoldRenderer->UpdateVBOs(); });
	GlobalEvents::gOnChartEdit.Add([](Chart* chart) { chart->mMineRenderer->UpdateVBOs(); });

	GlobalEvents::gOnSongCreate.Add([this](Song* song) { mMeasureLines.OnSongChange(song); });
}

void ImGuiEditor::Load()
{
	LoadIcon("Transform");
	LoadIcon("MeshRenderer");

	mReceptors.Initialize();
}

void ImGuiEditor::Update(const double dt)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	Window* win = WindowMgr->mCurrentWindow.get();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(*win);
	ImGui::NewFrame();

	Dockspace();
	MainMenu();
	
	SongPropertiesWindow();
	ChartPropertiesWindow();
	ChartListWindow();
	AdjustSyncWindow();
	mDancingBotWindow->OnGui();

	Profiler->Update();

	ImGui::EndFrame();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// HyperTwist
	if (mSong && mSong->get()->IsPlaying())
	{
		const double nextStep = GetNextStep(dt);
		if (!mPauseTimer.IsActive())
		{
			if (nextStep >= std::ceil(mCurrMeasure))
			{
				mBeatTick->get()->Play();
			}

			Note* NextNoteL = Editor->GetNextNote(mCurrMeasure);
			Note* NextNoteR = Editor->GetNextNote(nextStep);
			if ((NextNoteL && !NextNoteR) ||
				(NextNoteL && NextNoteR && Editor->GetNextNote(mCurrMeasure)->mPos != Editor->GetNextNote(nextStep)->mPos))
			{
				mNoteTick->get()->Play();
			}

			//this
			GfxMgr->mCam.SetPositionFromMeasure(mCurrMeasure);
			GlobalEvents::gOnSongUpdate.Broadcast(mCurrMeasure);
		}

		bool shouldStep = HandlePauseTimers(dt);

		if (shouldStep)
			mCurrMeasure = nextStep;
	}

	if (InputMgr->isKeyPressed(SDL_SCANCODE_SPACE))
		StartSong();

	if (InputMgr->isKeyPressed(SDL_SCANCODE_UP))
	{
		mCurrMeasure = mSnappers.GetNextMeasure(mCurrMeasure);
		GfxMgr->mCam.SetPositionFromMeasure(mCurrMeasure);
	}
	if (InputMgr->isKeyPressed(SDL_SCANCODE_DOWN))
	{
		mCurrMeasure = mSnappers.GetPrevMeasure(mCurrMeasure);
		GfxMgr->mCam.SetPositionFromMeasure(mCurrMeasure);
	}
	if (InputMgr->isKeyPressed(SDL_SCANCODE_LEFT))
		mSnappers.OnSnapMeasureChange(false);
	if (InputMgr->isKeyPressed(SDL_SCANCODE_RIGHT))
		mSnappers.OnSnapMeasureChange(true);


	if (InputMgr->isKeyPressed(SDL_SCANCODE_1))
		CreateOrDeleteNote(new Note(LeftDirection, mCurrMeasure));
	if (InputMgr->isKeyPressed(SDL_SCANCODE_2))
		CreateOrDeleteNote(new Note(DownDirection, mCurrMeasure));
	if (InputMgr->isKeyPressed(SDL_SCANCODE_3))
		CreateOrDeleteNote(new Note(UpDirection, mCurrMeasure));
	if (InputMgr->isKeyPressed(SDL_SCANCODE_4))
		CreateOrDeleteNote(new Note(RightDirection, mCurrMeasure));
}

void ImGuiEditor::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiEditor::Dockspace()
{
	// DockSpace
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &mbDockWindowOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("TestDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), mDockspaceFlags);
	}

	ImGui::End();
}

void ImGuiEditor::MainMenu()
{
	static bool ShowImGuiDemoWindow;
	static bool isSavingNewScene;

	if (ImGui::BeginMainMenuBar()) // MainMenu (top-horizontal bar)
	{
		if (ImGui::BeginMenu("File")) // MainMenu block
		{
			if (ImGui::MenuItem("New"))
			{
			}
			if (ImGui::MenuItem("Open"))
			{
				OPENFILENAMEW ofn;
				wchar_t fileName[MAX_PATH] = { 0 };

				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = fileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

				// Get the default language of the system
				LANGID languageID = GetUserDefaultLangID();

				// Determine the filters depending on the language
				if (languageID == MAKELANGID(LANG_SPANISH, SUBLANG_DEFAULT))
					ofn.lpstrFilter = L"Archivos compatibles (*.sm, *.ssc, *.ogg, *.mp3, *.wav)\0*.sm;*.ssc;*.ogg;*.mp3;*.wav\0Stepmania/ITG (*.sm)\0*.sm\0Stepmania 5 (*.ssc)\0*.ssc\0Archivo Ogg Vorbis (*.ogg)\0*.ogg\0Archivo MP3 (*.mp3)\0*.mp3\0Archivo Waveform (*.wav)\0*.wav\0Todos los archivos (*.*)\0*.*\0";
				else
					ofn.lpstrFilter = L"Compatible Files (*.sm, *.ssc, *.ogg, *.mp3, *.wav)\0*.sm;*.ssc;*.ogg;*.mp3;*.wav\0Stepmania/ITG (*.sm)\0*.sm\0Stepmania 5 (*.ssc)\0*.ssc\0Ogg Vorbis File (*.ogg)\0*.ogg\0MP3 File (*.mp3)\0*.mp3\0Waveform File (*.wav)\0*.wav\0All Files (*.*)\0*.*\0";

				if (GetOpenFileNameW(&ofn))
				{
					std::wstring wideString(fileName);
					mSong = ResourceMgr->Load<Song>(std::string(wideString.begin(), wideString.end()));
					ChangeDifficulty(mSong->get()->mCharts.rbegin()->second->mDifficultyCategory);
					GfxMgr->mCam.SetPositionFromMeasure(0.0f);
				}
			}
			if (mSong && ImGui::MenuItem("Save"))
				mSong->get()->SaveAsSSCSong();
 
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ShowImGuiDemoWindow = !ShowImGuiDemoWindow;
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (ShowImGuiDemoWindow)
		ImGui::ShowDemoWindow();
}

void ImGuiEditor::SetImGuiStyle()
{
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Tab] = ImVec4(0.95f, 0.4f, 0.4f, 1.0f); // Tab color
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.0f, 0.6f, 0.6f, 1.0f); // Tab hover color
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.8f, 0.8f, 0.0f, 1.0f); // Active tab color

	// Text and widget bg
	style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.80f, 0.2f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4f, 0.4f, 0.4, 0.4f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.07f, 0.69f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.12f, 0.45f, 0.69f);

	// Top Row
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.9f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 0.9f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.90f, 0.00f, 0.80f, 0.20f);
	
	//style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.90f, 0.55f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.5f, 0.5f, 0.5f, 0.6f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.6f, 0.5f, 0.5f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.6f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 0.8f);

	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.90f, 0.60f, 0.60f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.60f, 0.41f, 0.62f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.98f, 0.01f, 0.79f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.94f, 0.70f, 0.8f);

	style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.40f, 0.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.90f, 0.45f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.87f, 0.53f, 0.0f);

	style.Colors[ImGuiCol_Separator] = ImVec4(0.40f, 0.40f, 0.40f, 0.60f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);

	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);

	style.Colors[ImGuiCol_Tab] = ImLerp(style.Colors[ImGuiCol_Header], style.Colors[ImGuiCol_TitleBgActive], 0.80f);
	style.Colors[ImGuiCol_TabHovered] = style.Colors[ImGuiCol_HeaderHovered];
	style.Colors[ImGuiCol_TabActive] = ImLerp(style.Colors[ImGuiCol_HeaderActive], style.Colors[ImGuiCol_TitleBgActive], 0.60f);
	style.Colors[ImGuiCol_TabUnfocused] = ImLerp(style.Colors[ImGuiCol_Tab], style.Colors[ImGuiCol_TitleBg], 0.80f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(style.Colors[ImGuiCol_TabActive], style.Colors[ImGuiCol_TitleBg], 0.40f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.0f, 0.70f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 0.00f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	style.Colors[ImGuiCol_NavHighlight] = style.Colors[ImGuiCol_HeaderHovered];
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	style.TabBorderSize = 0.50f; // Border size around tabs
	style.TabRounding = 0.5f; // Rounding of the tabs

	style.WindowPadding = ImVec2(10, 10);
	style.WindowRounding = 5.0f;
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 4.0f;
	style.ItemSpacing = ImVec2(8, 7);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f;
}

void ImGuiEditor::StartSong()
{
	mStartTimer = PauseTimer(mSong->get()->mOffset);
	mCurrMeasure = 0.0f;

	mSong->get()->Play(mCurrMeasure);

	GlobalEvents::gOnSongPlay.Broadcast();
}

void ImGuiEditor::ChangeDifficulty(const ChartDifficulty Difficulty)
{
	mCurrDifficulty = Difficulty;
	auto charts = mSong->get()->mCharts;
	for (auto chartInfo : charts)
	{
		Chart* currChart = chartInfo.second;
		const bool shouldBeVisible = currChart->mDifficultyCategory == Difficulty;
		currChart->mNoteRenderer->mbIsVisible = shouldBeVisible;
		currChart->mHoldRenderer->mbIsVisible = shouldBeVisible;
		currChart->mMineRenderer->mbIsVisible = shouldBeVisible;
	}
}

Chart* ImGuiEditor::GetCurrChart()
{
	if (mSong == nullptr)
		return nullptr;
	
	return mSong->get()->mCharts[mCurrDifficulty];
}

Note* ImGuiEditor::GetNextNote(const double currMeasure)
{
	Note* nextNote = nullptr;
	Note* tempNote = new Note(LeftDirection, currMeasure);
	auto& noteList = GetCurrNotes();
	auto measureNotes = noteList.upper_bound(tempNote);

	// @TODO: Multiple notes instead of single note
	if (measureNotes != noteList.end())
	{
		nextNote = *measureNotes;
	}

	delete tempNote;
	return nextNote;
}

double ImGuiEditor::GetNextStop(const double currMeasure)
{
	auto stops = mSong->get()->mStops;
	auto nextStop = stops.upper_bound(currMeasure);

	if (nextStop != stops.end())
		return nextStop->first;

	return FLT_MAX;
}

double ImGuiEditor::GetNextStep(const double dt)
{
	const double currBPM = mSong->get()->GetBPMAt(mCurrMeasure);
	const double measureSpeed = currBPM / 60.0;
	const double step = dt * measureSpeed; // measure = (t (in seconds) * BPM) / (4 measures * 60s)
	return mCurrMeasure + step;
}

void ImGuiEditor::CreateOrDeleteNote(Note* newNote)
{
	auto& noteList = mSong->get()->mCharts[mCurrDifficulty]->mNotes;
		
	auto it = std::find_if(noteList.begin(), noteList.end(), [newNote](const Note* note) {
		return note->mPos == newNote->mPos && note->mDir == newNote->mDir;
		});

	if (it != noteList.end())
	{
		noteList.erase(it);
		delete newNote;
	}
	else
	{
		noteList.insert(newNote);
	}

	GlobalEvents::gOnChartEdit.Broadcast(mSong->get()->mCharts[mCurrDifficulty]);
}

void ImGuiEditor::SongPropertiesWindow()
{
	if (ImGui::Begin("Song Properties") && mSong != nullptr)
	{
		Song* song = mSong->get();

		if (Texture* banner = song->GetBanner())
		{
			GLuint texID = song->GetBanner()->GetID();
			ImGui::SameLine();
			ImGui::Image(reinterpret_cast<ImTextureID>(texID), { ImGui::GetWindowSize().x, ImGui::GetWindowSize().x / 3.0f }, { 0,1 }, { 1,0 });
		}

		ImGui::OnGui("Title", song->mTitle);
		ImGui::OnGui("Subtitle", song->mSubtitle);
		ImGui::OnGui("Artist", song->mArtist);
		ImGui::OnGui("Credit", song->mCredit);
		ImGui::Separator();
		ImGui::OnGui("Music", song->mSongPath);
		ImGui::OnGui("BG", song->mBackgroundPath);
		ImGui::OnGui("Banner", song->mBannerPath);
		ImGui::OnGui("CD Title", song->mCDTitlePath);

		// ImGui::OnGui("Preview", song->mTitle);
		// ImGui::OnGui("Disp. BPM", song->mTitle);
	}

	ImGui::End();
}

void ImGuiEditor::ChartListWindow()
{
	if (ImGui::Begin("Chart List") && mSong != nullptr)
	{
		for (auto chartInfo : mSong->get()->mCharts)
		{
			Chart* chart = chartInfo.second;

			float hueVal = 0.0f;
			std::string text;

			switch (chart->mDifficultyCategory)
			{
			case ChartDifficulty::Beginner:
				hueVal = 0.51f;
				text = "  Begginer";
				break;
			case ChartDifficulty::Easy:
				hueVal = 0.14f;
				text = "    Easy  ";
				break;
			case ChartDifficulty::Medium:
				hueVal = 1.0f;
				text = "   Medium ";
				break;
			case ChartDifficulty::Hard:
				hueVal = 0.32f;
				text = "    Hard  ";
				break;
			case ChartDifficulty::Challenge:
				hueVal = 0.82f;
				text = " Challenge";
				break;
			}

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.0f });
			ImGui::SetNextItemWidth(-1);
			text += " - " + chart->mStepArtist + " - " + std::to_string(chart->mDifficultyVal);
			ImGui::PushID(chart->mDifficultyCategory);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hueVal, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hueVal, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hueVal, 0.8f, 0.8f));
			if (ImGui::Button(text.c_str(), { -1.0f, 0.0f }))
				ChangeDifficulty(chart->mDifficultyCategory);

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();
			ImGui::PopFont();
			ImGui::PopID();
		}
	}

	ImGui::End();
}

void ImGuiEditor::ChartPropertiesWindow()
{
	Chart* pChart = GetCurrChart();
	if (!pChart)
	{
		return;
	}

	if (ImGui::Begin("Chart Properties"))
	{
		// ImGui::Combo("Difficulty", )
		//
		ImGui::OnGui("Difficulty", pChart->mDifficultyVal, 0, 25);
		ImGui::SameLine();
		if (ImGui::Button("Set"))
		{

		}
		ImGui::OnGui("Step Artist", pChart->mStepArtist);

		// Chart Type (Dance Single)
		// Difficulty   Medium   9  (cog)
		// Step Artist     Balea
		ImGui::Separator();

		// ()      Note Information
		// steps ()     jumps ()    mines ()
		// holds ()     rolls ()    warps ()
		// Note density: 2.4 NPS   Stream measures: 0

		ImVec2 textSize = ImGui::CalcTextSize("Note info");
		float windowWidth = ImGui::GetWindowSize().x;
		float centerPosX = (windowWidth - textSize.x) / 2.0f;
		ImGui::SetCursorPosX(centerPosX);
		ImGui::TextColored({ 0.8f, 1.0f, 0.8f, 1.0f }, "Note info");

		ImGui::Columns(3, "NoteInfo", false);


		// Get notes
		ImGui::Text("steps");
		ImGui::SameLine();
		std::multiset<Note*, NoteCompare> notes = pChart->GetAllNormalNotes();
		if (ImGui::Button(std::to_string(notes.size()).c_str(), {-1.0f, 0.0f}))
		{

		}
		ImGui::NextColumn();

		// Get jumps
		ImGui::Text("jumps");
		ImGui::SameLine();
		std::vector<std::vector<Note*>> jumps = pChart->GetAllJumps();
		if (ImGui::Button(std::to_string(jumps.size()).c_str(), { -1.0f, 0.0f }))
		{

		}
		ImGui::NextColumn();

		// Get mines
		ImGui::Text("mines");
		ImGui::SameLine();
		std::multiset<MineNote*, NoteCompare> mines = pChart->GetAllMineNotes();
		if (ImGui::Button(std::to_string(mines.size()).c_str(), { -1.0f, 0.0f }))
		{

		}
		ImGui::NextColumn();

		// Get holds
		ImGui::Text("holds");
		ImGui::SameLine();
		std::multiset<HoldNote*, NoteCompare> holds = pChart->GetAllHoldNotes();
		if (ImGui::Button(std::to_string(holds.size()).c_str(), { -1.0f, 0.0f }))
		{

		}
		ImGui::NextColumn();

		// Get rolls
		ImGui::Text("rolls");
		ImGui::SameLine();
		std::multiset<RollNote*, NoteCompare> rolls = pChart->GetAllRollNotes();
		if (ImGui::Button(std::to_string(rolls.size()).c_str(), { -1.0f, 0.0f }))
		{

		}
		ImGui::NextColumn();

		// Get warps
		ImGui::Text("warps");
		ImGui::SameLine();
		if (ImGui::Button("0", { -1.0f, 0.0f }))
		{

		}
		ImGui::Columns(1);

		std::string noteDensityStr = std::string("Note density: ") + "6.1" + " NPS";
		std::string streamMeasuresStr = std::string("Stream measures: ") + "5";
		ImGui::Columns(2, "Note data", false);

		windowWidth /= 2.0f;
		textSize = ImGui::CalcTextSize(noteDensityStr.c_str());
		centerPosX = (windowWidth - textSize.x) / 2.0f;
		ImGui::SetCursorPosX(centerPosX);
		ImGui::Text(noteDensityStr.c_str());
		ImGui::NextColumn();

		textSize = ImGui::CalcTextSize(streamMeasuresStr.c_str());
		centerPosX += windowWidth;
		ImGui::SetCursorPosX(centerPosX);
		ImGui::Text(streamMeasuresStr.c_str());
		ImGui::Columns(1);

		ImGui::Separator();
		// ()      Stream breakdown
		windowWidth *= 2.0f;
		textSize = ImGui::CalcTextSize("Stream breakdown");
		centerPosX = (windowWidth - textSize.x) / 2.0f;
		ImGui::SetCursorPosX(centerPosX);
		ImGui::TextColored({ 0.8f, 1.0f, 0.8f, 1.0f }, "Stream breakdown");
	}

	ImGui::End();
}

void ImGuiEditor::AdjustSyncWindow()
{
	if (!mSong)
		return;

	Song* pSong = mSong->get();
	if (ImGui::Begin("Adjust Sync"))
	{
		ImGui::OnGui("Music Offset", pSong->mOffset);
		ImGui::OnGui("Initial BPM", pSong->mOffset);

		ImGui::Text("Move first beat");
		ImGui::SameLine();
		ImGui::Button("-1");
		ImGui::SameLine();
		ImGui::Button("-0.5");
		ImGui::SameLine();
		ImGui::Button("+0.5");
		ImGui::SameLine();
		ImGui::Button("+1");

		ImGui::Separator();
		ImVec2 textSize = ImGui::CalcTextSize("Automatic BPM detection");
		float windowWidth = ImGui::GetWindowSize().x;
		float centerPosX = (windowWidth - textSize.x) / 2.0f;
		ImGui::SetCursorPosX(centerPosX);
		ImGui::Text("Automatic BPM detection");
		if (ImGui::Button("Find BPM"))
		{
			const char* input_filename = pSong->mSongPath.c_str();
			uint_t win_size = 1024; // window size for the analysis
			uint_t hop_size = win_size / 4; // hop size for the analysis
			uint_t samplerate = 44100; // typical sample rate; set to 0 to use the file's original rate

			// Create tempo detection object with proper sample rate
			aubio_tempo_t* tempo = new_aubio_tempo("default", win_size, hop_size, samplerate);

			// Create a new source object for .ogg file
			aubio_source_t* source = new_aubio_source(input_filename, samplerate, hop_size);
			if (!source) {
				std::cerr << "Could not open input file " << input_filename << std::endl;
				ImGui::End();
				return;
			}

			samplerate = aubio_source_get_samplerate(source);

			float bpm = 0.0;
			uint_t read = 0;
			fvec_t* in = new_fvec(hop_size); // input audio buffer
			fvec_t* out = new_fvec(1); // output (tempo)

			// Process the whole file
			do {
				// Read from the source
				aubio_source_do(source, in, &read);
				aubio_tempo_do(tempo, in, out);

				if (out->data[0] != 0) {
					bpm = aubio_tempo_get_bpm(tempo);
				}
			} while (read == hop_size);

			std::cout << "Estimated BPM: " << bpm << std::endl;

			// Clean up memory
			del_aubio_tempo(tempo);
			del_fvec(in);
			del_fvec(out);
			del_aubio_source(source);
			aubio_cleanup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Apply BPM"))
		{

		}
	}

	ImGui::End();
}

void ImGuiEditor::NewChartPopup()
{
	// Chart Type (Dance Single)
	// Difficulty   Challenge>   9
	// Step Artist  Balea
	//                      Create
}

void ImGuiEditor::LoadIcon(const std::string& name)
{
	mIcons[name] = ResourceMgr->LoadFromBasePath<Texture>("engine/icons/" + name + ".png");
}

bool ImGuiEditor::HandlePauseTimers(const double dt)
{
	bool shouldStep = true;

	float startTime = mStartTimer.GetTime();
	if (startTime > 0.0f)
	{
		mTimePlayedInSeconds += dt;
		startTime -= dt;

		if (startTime < 0.0f)
		{
			mSong->get()->Play(mTimePlayedInSeconds);
		}

		mStartTimer.SetTime(startTime);
	}

	float pauseTime = mPauseTimer.GetTime();
	if (pauseTime > 0.0f)
	{
		pauseTime -= dt;
		mPauseTimer.SetTime(pauseTime);
		shouldStep = false;
	}

	const float currStop = GetNextStop(mCurrMeasure);
	const float nextStep = GetNextStep(dt);
	if (nextStep >= currStop)
	{
		const float stopTime = mSong->get()->mStops[currStop];
		if (stopTime < 0)
		{
			shouldStep = false;
			mCurrMeasure = GetNextStep(-stopTime);
		}
		else
		{
			mPauseTimer.SetTime(stopTime);
		}
	}

	return shouldStep;
}

void ImGuiEditor::HandleEditorInput()
{
	if (InputMgr->isButtonDown(InputMgr->eButtonRight))
	{
		glm::ivec2 mouseMovement = InputMgr->GetMouseMovement();
		GfxMgr->mCam.Move({ 0.0f, mouseMovement.y * 0.05f });
	}

	if (InputMgr->isKeyDown(SDL_SCANCODE_N))
	{
		gGlobalVariables.mZoom += TimeMgr->deltaTime * 50;
	}

	if (InputMgr->isKeyDown(SDL_SCANCODE_M))
	{
		gGlobalVariables.mZoom -= TimeMgr->deltaTime * 50;
	}

	gGlobalVariables.mZoom = std::clamp(gGlobalVariables.mZoom, 50.0f, 200.0f);

	Editor->mReceptors.Update(GfxMgr->mCam.mPos);
	Editor->mSnappers.Update(GfxMgr->mCam.mPos);
}
