#pragma once
#include "Chart.h"
#include "Audio/Audio.h"
#include "Graphics/Texture.h"
#include "Resources/ResourceMgr.h"
#include "Game/SongInfo.h"

#include <map>

struct Song
{
	enum class FileType 
	{
		SM,
		SSC,
		SMD_SCD,
		Unknown
	};

	Song(const std::string& path);

	std::string GetPath() const { return mPath; }

	void Play(const f32 measure);

	f32 GetBPMAt(const f32 time);

	bool IsPlaying() { return mIsPlaying; }

	Resource<Audio>* GetSong();
	Resource<Texture>* GetBanner();
	Resource<Texture>* GetBackground();
	Resource<Texture>* GetCDTitle();

	void SaveAsSSCSong(const std::string& outSong = "");

	void SaveToSMD(const std::string& outSong = "");
	void SaveToSCD(const std::string& outSong = "");

	void LoadChartsSCD();

private:
	void ProcessSMSong(std::istringstream& file);
	void ProcessSSCSong(std::istringstream& file);

	Chart* ProcessSMChart(std::istringstream& file);
	Chart* ProcessSSCChart(std::istringstream& file);
	

	void ProcessSMD(std::istringstream& file);
	void ProcessSCD(std::istringstream& file);
	Chart* ProcessSCDChart(std::istringstream& file);

public:
	FileType mFileType = FileType::Unknown;

	SongInfo mSongInfo;

	std::map<ChartDifficulty, u8> mChartDifficulties;
	std::map<ChartDifficulty, Chart*> mCharts;

private:
	std::string mPath;
	std::string mNoteDataPath; // If main file is a data file and contains a note data file.
	Resource<Audio>* mSong = nullptr;
	Resource<Texture>* mBanner = nullptr;
	Resource<Texture>* mBackground = nullptr;
	Resource<Texture>* mCDTitle = nullptr;

	bool mIsPlaying = false;
};