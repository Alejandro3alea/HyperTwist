#pragma once
#include "Chart.h"
#include "Audio/Audio.h"
#include "Graphics/Texture.h"
#include "Resources/ResourceMgr.h"

#include <map>

using SpeedValues = std::tuple<float, float, bool>;
using TimeSignatureValues = std::pair<int, int>;

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

	void Play(const float measure);

	float GetBPMAt(const float time);

	bool IsPlaying() { return mIsPlaying; }

	Audio* GetSong();
	Texture* GetBanner();
	Texture* GetBackground();
	Texture* GetCDTitle();

	void SaveAsSSCSong(const std::string& outSong = "");

	void SaveToSMD(const std::string& outSong = "");
	void SaveToSCD(const std::string& outSong = "");

	void LoadChartsSCD();

private:
	void GetResources();

	void ProcessSMSong(std::istringstream& file);
	void ProcessSSCSong(std::istringstream& file);

	Chart* ProcessSMChart(std::istringstream& file);
	Chart* ProcessSSCChart(std::istringstream& file);
	

	void ProcessSMD(std::istringstream& file);
	void ProcessSCD(std::istringstream& file);
	Chart* ProcessSCDChart(std::istringstream& file);

public:
	FileType mFileType = FileType::Unknown;
	std::string mTitle = "unknown";
	std::string mSubtitle;
	std::string mArtist;
	std::string mTitleTranslit;
	std::string mSubtitleTranslit;
	std::string mArtistTranslit;
	std::string mGenre;
	std::string mPlatform;
	std::string mPlatformVersion;
	std::string mCredit;
	std::string mSongPath;
	std::string mBannerPath;
	std::string mBackgroundPath;
	std::string mCDTitlePath;
	f32 mSampleStart = 0.0f;
	f32 mSampleLength = 0.0f;
	bool mSelectable = true;
	f32 mOffset = 0.0f;

	std::map<f32, f32> mBPMs;
	std::map<f32, f32> mStops;
	std::map<f32, SpeedValues> mSpeeds;
	std::map<f32, f32> mScrolls;

	std::map<f32, i32> mTickCounts;
	std::map<f32, TimeSignatureValues> mTimeSignatures;
	std::map<f32, std::string> mLabels;
	std::map<f32, i32> mCombos; // @TODO: Figure out how this works

	f32 mDisplayBPM; 
	
	std::string mOrigin; // @TODO: Figure out how this works   #ORIGIN:;
	std::string mPreviewVID; // @TODO: Figure out how this works   #PREVIEWVID:;
	std::string mJacket; // @TODO: Figure out how this works   #JACKET:;
	std::string mCDImage; // @TODO: Figure out how this works   #CDIMAGE:;
	std::string mDiscImage; // @TODO: Figure out how this works   #DISCIMAGE:;
	std::map<f32, std::string> mBGChanges; // @TODO: Figure out how this works   #BGCHANGES:;
	std::string mFGChanges; // @TODO: Figure out how this works   #FGCHANGES:;

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