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
	Song(const std::string& path);

	std::string GetPath() const { return mPath; }

	void Play(const float measure);

	float GetBPMAt(const float time);

	bool IsPlaying() { return mIsPlaying; }

	Audio* GetSong()		 { if (mSong) return mSong->get();				else return nullptr; }
	Texture* GetBanner()	 { if (mBanner) return mBanner->get();			else return nullptr; }
	Texture* GetBackground() { if (mBackground) return mBackground->get();	else return nullptr; }
	Texture* GetCDTitle()	 { if (mCDTitle) return mCDTitle->get();		else return nullptr; }

	void SaveAsSSCSong();

private:
	void GetResources();

	void ProcessSMSong(std::istringstream& file);
	void ProcessSSCSong(std::istringstream& file);

	Chart* ProcessSMChart(std::istringstream& file);
	Chart* ProcessSSCChart(std::istringstream& file);
	

	void ProcessSMD(std::istringstream& file);
	void ProcessSMN(std::istringstream& file);
	Chart* ProcessSMNChart(std::istringstream& file);


public:
	std::string mTitle = "unknown";
	std::string mSubtitle;
	std::string mArtist;
	std::string mTitleTranslit;
	std::string mSubtitleTranslit;
	std::string mArtistTranslit;
	std::string mGenre;
	std::string mCredit;
	std::string mSongPath;
	std::string mBannerPath;
	std::string mBackgroundPath;
	std::string mCDTitlePath;
	float mSampleStart = 0.0f;
	float mSampleLength = 0.0f;
	bool mSelectable = true;
	float mOffset = 0.0f;

	std::map<float, float> mBPMs;
	std::map<float, float> mStops;
	std::map<float, SpeedValues> mSpeeds;
	std::map<float, float> mScrolls;

	std::map<float, int> mTickCounts;
	std::map<float, TimeSignatureValues> mTimeSignatures;
	std::map<float, std::string> mLabels;
	std::map<float, int> mCombos; // @TODO: Figure out how this works

	float mDisplayBPM; 
	
	std::string mOrigin; // @TODO: Figure out how this works   #ORIGIN:;
	std::string mPreviewVID; // @TODO: Figure out how this works   #PREVIEWVID:;
	std::string mJacket; // @TODO: Figure out how this works   #JACKET:;
	std::string mCDImage; // @TODO: Figure out how this works   #CDIMAGE:;
	std::string mDiscImage; // @TODO: Figure out how this works   #DISCIMAGE:;
	std::map<float, std::string> mBGChanges; // @TODO: Figure out how this works   #BGCHANGES:;
	std::string mFGChanges; // @TODO: Figure out how this works   #FGCHANGES:;

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