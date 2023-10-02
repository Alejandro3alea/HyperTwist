#pragma once
#include "Chart.h"
#include "Audio/Audio.h"
#include "Graphics/Texture.h"
#include "Resources/ResourceMgr.h"

#include <map>

struct Song
{
	Song(const std::string& path);

	std::string GetPath() const { return mPath; }

	void Play();

	float GetBPMAt(const float time);

	bool IsPlaying() { return mIsPlaying; }

	Audio*	 GetSong()			{ if (mSong) return mSong->get();				else return nullptr; }
	Texture* GetBanner()		{ if (mBanner) return mBanner->get();			else return nullptr; }
	Texture* GetBackground()	{ if (mBackground) return mBackground->get();	else return nullptr; }
	Texture* GetCDTitle()		{ if (mCDTitle) return mCDTitle->get();			else return nullptr; }

private:
	void GetResources();

	void ProcessSMSong(std::istringstream& file);
	void ProcessSSCSong(std::istringstream& file);

	Chart* ProcessSMChart(std::istringstream& file);
	Chart* ProcessSSCChart(std::istringstream& file);

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
	//std::vector<float> mStops;

	std::vector<Chart*> mCharts;

private:
	std::string mPath;
	Resource<Audio>* mSong = nullptr;
	Resource<Texture>* mBanner = nullptr;
	Resource<Texture>* mBackground = nullptr;
	Resource<Texture>* mCDTitle = nullptr;

	bool mIsPlaying = false;
};