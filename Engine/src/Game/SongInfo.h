#pragma once
#include "Misc/DataTypes.h"

#include <string>
#include <map>

using SpeedValues = std::tuple<f32, f32, bool>;
using TimeSignatureValues = std::pair<i32, i32>;

struct SongInfo
{
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
};