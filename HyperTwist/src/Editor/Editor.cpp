#include "Editor.h"
#include "WindowMgr.h"
#include "Graphics/Renderable.h"
#include "Input/InputMgr.h"
#include "Tools/Profiler/Profiler.h"
#include "ImGuiWidgets.h"

#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include <Windows.h>

#undef LoadIcon

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

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	Window* win = WindowMgr->mCurrentWindow.get();
	// Setup Platform/Renderer bindings

	ImGui_ImplSDL2_InitForOpenGL(*win, win);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// HyperTwist
	mBeatTick = ResourceMgr->Load<Audio>("data/engine/audio/sfx/BeatTick.wav");
	mNoteTick = ResourceMgr->Load<Audio>("data/engine/audio/sfx/NoteTick.wav");

	Profiler->Initialize();
}

void ImGuiEditor::Load()
{
	LoadIcon("Transform");
	LoadIcon("MeshRenderer");

	mReceptors.Initialize();
}

void ImGuiEditor::Update(const float dt)
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
	DancingBotWindow();

	Profiler->Update();

	ImGui::EndFrame();

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	
	// HyperTwist
	if (mOffsetTime < 0.0f)
		mOffsetTime += dt;
	else if (mSong && mSong->get()->IsPlaying())
	{
		const float currBPM = mSong->get()->GetBPMAt(mCurrMeasure);
		const float measureSpeed = currBPM / 60.0f;
		const float step = dt * measureSpeed; // measure = (t (in seconds) * BPM) / (4 measures * 60s)
		const float nextStep = mCurrMeasure + step;
		if (nextStep >= std::ceil(mCurrMeasure))
		{
			mBeatTick->get()->Play();
		}
		mCurrMeasure = nextStep;
	}

	if (InputMgr->isKeyPressed(SDL_SCANCODE_SPACE))
		StartSong();
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

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
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

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
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

				// Obtener el idioma actual del sistema
				LANGID languageID = GetUserDefaultLangID();

				// Determinar las cadenas de filtro según el idioma
				if (languageID == MAKELANGID(LANG_SPANISH, SUBLANG_DEFAULT))
					ofn.lpstrFilter = L"Archivos compatibles (*.sm, *.ssc, *.ogg, *.mp3, *.wav)\0*.sm;*.ssc;*.ogg;*.mp3;*.wav\0Stepmania/ITG (*.sm)\0*.sm\0Stepmania 5 (*.ssc)\0*.ssc\0Archivo Ogg Vorbis (*.ogg)\0*.ogg\0Archivo MP3 (*.mp3)\0*.mp3\0Archivo Waveform (*.wav)\0*.wav\0Todos los archivos (*.*)\0*.*\0";
				else
					ofn.lpstrFilter = L"Compatible Files (*.sm, *.ssc, *.ogg, *.mp3, *.wav)\0*.sm;*.ssc;*.ogg;*.mp3;*.wav\0Stepmania/ITG (*.sm)\0*.sm\0Stepmania 5 (*.ssc)\0*.ssc\0Ogg Vorbis File (*.ogg)\0*.ogg\0MP3 File (*.mp3)\0*.mp3\0Waveform File (*.wav)\0*.wav\0All Files (*.*)\0*.*\0";

				if (GetOpenFileNameW(&ofn))
				{
					//MessageBoxW(NULL, fileName, L"Archivo seleccionado", MB_OK);
					std::wstring wideString(fileName);
					mSong = ResourceMgr->Load<Song>(std::string(wideString.begin(), wideString.end()));
					ChangeDifficulty(mSong->get()->mCharts[	mSong->get()->mCharts.size() - 1 ]->mDifficulty);
					mMeasureLines.OnSongChange(mSong->get());
				}
			}
 
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

void ImGuiEditor::StartSong()
{
	mSong->get()->Play();
	mCurrMeasure = 0.0f;

	mOffsetTime = mSong->get()->mOffset;
}

void ImGuiEditor::ChangeDifficulty(const ChartDifficulty Difficulty)
{
	mCurrDifficulty = Difficulty;
	auto Charts = mSong->get()->mCharts;
	for (auto CurrChart : Charts)
	{
		bool IsVisible = CurrChart->mDifficulty == Difficulty;
		for (unsigned i = 0; i < CurrChart->mNotes.size(); i++)
			CurrChart->mNotes[i]->mAtlas.mbIsVisible = IsVisible;
	}
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
		for (auto chart : mSong->get()->mCharts)
		{
			float hueVal = 0.0f;
			std::string text;

			switch (chart->mDifficulty)
			{
			case ChartDifficulty::Beginner:
				hueVal = 0.51f;
				text = "Begginer\t";
				break;
			case ChartDifficulty::Easy:
				hueVal = 0.14f;
				text = "Easy\t";
				break;
			case ChartDifficulty::Medium:
				hueVal = 1.0f;
				text = "Medium\t";
				break;
			case ChartDifficulty::Hard:
				hueVal = 0.32f;
				text = "Hard\t";
				break;
			case ChartDifficulty::Challenge:
				hueVal = 0.82f;
				text = "Challenge\t";
				break;
			}

			text += chart->mStepArtist + "\t" + std::to_string(chart->mDifficultyVal);
			ImGui::PushID(chart->mDifficulty);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hueVal, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(hueVal, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(hueVal, 0.8f, 0.8f));
			if (ImGui::Button(text.c_str()))
				ChangeDifficulty(chart->mDifficulty);

			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}
	}

	ImGui::End();
}

void ImGuiEditor::ChartPropertiesWindow()
{
	if (ImGui::Begin("Chart Properties"))
	{
		// Chart Type (Dance Single)
		// Difficulty   Medium   9  (cog)
		// Step Artist     Balea
		ImGui::Separator();
		// ()      Note Information
		// steps ()     jumps ()    mines ()
		// holds ()     rolls ()    warps ()
		// Note density: 2.4 NPS   Stream measures: 0
		ImGui::Separator();
		// ()      Stream breakdown
	}

	ImGui::End();
}

void ImGuiEditor::DancingBotWindow()
{
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
	mIcons[name] = ResourceMgr->Load<Texture>("data/engine/icons/" + name + ".png");
}
