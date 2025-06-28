#include "Song.h"
#include "Misc/Requires.h"
#include "GlobalEvents.h"
#include "GameVariables.h"
#include "Graphics/GfxMgr.h"
#include "Utils/GameUtils.h"
#include "Audio/Audio.h"

#include <sstream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <iomanip>

void RemoveSingleLineComments(std::string& text)
{
    size_t pos = text.find("//");
    while (pos != std::string::npos) 
    {
        size_t endPos = text.find('\n', pos);
        if (endPos == std::string::npos)
        {
            text.erase(pos);
            break;
        }

        text.erase(pos, endPos - pos);
        pos = text.find("//", pos);
    }
}

void RemoveMultiLineComments(std::string& text)
{
    size_t startPos = text.find("/*");
    while (startPos != std::string::npos)
    {
        size_t endPos = text.find("*/", startPos + 2);
        if (endPos == std::string::npos)
        {
            text.erase(startPos);
            break;
        }
        text.erase(startPos, endPos - startPos + 2);
        startPos = text.find("/*", startPos);
    }
}

void RemoveCarriageReturns(std::string& text)
{
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
}

void RemoveBlankLines(std::string& text)
{
    std::istringstream iss(text);
    std::string line;
    std::string result;

    while (std::getline(iss, line)) 
    {
        if (!line.empty()) 
        {
            result += line + "\n";
        }
    }

    text = result;
}

void RemoveContents(std::string& text)
{
    RemoveSingleLineComments(text);
    RemoveMultiLineComments(text);
    RemoveCarriageReturns(text);
    RemoveBlankLines(text);
}

Song::Song(const std::string& path)
{
    std::string tPath(path);
    std::replace(tPath.begin(), tPath.end(), '\\', '/');
    size_t lastSpacePos = tPath.find_last_of('/');
    if (lastSpacePos != std::string::npos)
        mPath = tPath.substr(0, lastSpacePos) + '/';

	std::ifstream file(path);
	Requires(file.is_open() && file.good(), "File not read in " + path);

    std::string fileContents((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    RemoveContents(fileContents);
    std::istringstream cleanContents(fileContents);

    std::string extension = ResourceMgr->GetExtension(path);
    if (extension == "sm")
        ProcessSMSong(cleanContents);
    else if (extension == "ssc")
        ProcessSSCSong(cleanContents);
    else if (extension == "smd")
        ProcessSMD(cleanContents);

	file.close();
    
    GlobalEvents::gOnSongCreate.Broadcast(this);

    GetResources();
}

void Song::Play(const float measure)
{
    if (measure >= mOffset)
    {
        mSong->get()->Play();
        mSong->get()->SetPosition(measure - mOffset);
    }
    mIsPlaying = true;
}

#undef max
float Song::GetBPMAt(const float time)
{
    switch (mBPMs.size())
    {
    case 0:
        return std::numeric_limits<float>::max();
    case 1:
        return mBPMs.begin()->second;
    default:
        auto it = mBPMs.begin();
        std::advance(it, 1);

        for (; it != mBPMs.end(); ++it)
        {
            if (time < it->first)
            {
                auto prevIt = --it;
                return prevIt->second;
            }
        }

        return mBPMs.rbegin()->second;
    }
}

Audio* Song::GetSong()
{
    if (mSong) 
        return mSong->get();

    mSong = ResourceMgr->Load<Audio>(mPath + mSongPath, AudioType::BGM);

    return mSong->get();
}

Texture* Song::GetBanner()
{
    if (mBanner)
        return mBanner->get();

    mBanner = ResourceMgr->Load<Texture>(mPath + mBannerPath);

    return mBanner->get();
}

Texture* Song::GetBackground()
{
    if (mBackground)
        return mBackground->get();

    mBackground = ResourceMgr->Load<Texture>(mPath + mBackgroundPath);

    return mBackground->get();
}

Texture* Song::GetCDTitle()
{
    if (mCDTitle)
        return mCDTitle->get();

    mCDTitle = ResourceMgr->Load<Texture>(mPath + mCDTitlePath);

    return mCDTitle->get();
}

void Song::GetResources()
{
    // There should be ALWAYS a song path (No audios, no party)
    //mSong = ResourceMgr->Load<Audio>(mPath + mSongPath, AudioType::BGM);

    /*if (!mBannerPath.empty())
        mBanner     = ResourceMgr->Load<Texture>(mPath + mBannerPath);
    if (!mBackgroundPath.empty())
    {
        mBackground = ResourceMgr->Load<Texture>(mPath + mBackgroundPath);
    }
    if (!mCDTitlePath.empty())
        mCDTitle    = ResourceMgr->Load<Texture>(mPath + mCDTitlePath);*/
}

void Song::ProcessSMSong(std::istringstream& file)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '/') // Ignore empty lines and comment lines
            continue;

        // Extract the key and value from each line
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            bool isEndOfCommand = false;
            if (value.find(';') != std::string::npos)
            {
                isEndOfCommand = true;
                value.pop_back();
            }

            // Store the value in the appropriate member of the SongData struct
            if (key == "#TITLE")
                mTitle = value;
            else if (key == "#SUBTITLE")
                mSubtitle = value;
            else if (key == "#ARTIST")
                mArtist = value;
            else if (key == "#TITLETRANSLIT")
                mTitleTranslit = value;
            else if (key == "#SUBTITLETRANSLIT")
                mSubtitleTranslit = value;
            else if (key == "#ARTISTTRANSLIT")
                mArtistTranslit = value;
            else if (key == "#GENRE")
                mGenre = value;
            else if (key == "#CREDIT")
                mCredit = value;
            else if (key == "#BANNER")
                mBannerPath = value;
            else if (key == "#BACKGROUND")
                mBackgroundPath = value;
            else if (key == "#CDTITLE")
                mCDTitlePath = value;
            else if (key == "#MUSIC")
                mSongPath = value;
            else if (key == "#OFFSET")
                mOffset = std::stof(value);
            else if (key == "#SAMPLESTART")
                mSampleStart = std::stof(value);
            else if (key == "#SAMPLELENGTH")
                mSampleLength = std::stof(value);
            else if (key == "#SELECTABLE")
                mSelectable = value == "YES";
            else if (key == "#BPMS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float bpm = std::stof(part.substr(equalPos + 1));

                        mBPMs[measurePos] = bpm;
                    }
                }
            }
            else if (key == "#STOPS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float stopTime = std::stof(part.substr(equalPos + 1));

                        mStops[measurePos] = stopTime;
                    }
                }
            }
            else if (key == "#NOTES")
            {
                if (Chart* newChart = ProcessSMChart(file))
                    mCharts[newChart->mDifficultyCategory] = newChart;
            }
        }
    }
}

void Song::ProcessSSCSong(std::istringstream& file)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '/') // Ignore empty lines and comment lines
            continue;

        // Extract the key and value from each line
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            bool isEndOfCommand = false;
            if (value.find(';') != std::string::npos)
            {
                isEndOfCommand = true;
                value.pop_back();
            }

            // Store the value in the appropriate member of the SongData struct
            if (key == "#TITLE")
                mTitle = value;
            else if (key == "#SUBTITLE")
                mSubtitle = value;
            else if (key == "#ARTIST")
                mArtist = value;
            else if (key == "#TITLETRANSLIT")
                mTitleTranslit = value;
            else if (key == "#SUBTITLETRANSLIT")
                mSubtitleTranslit = value;
            else if (key == "#ARTISTTRANSLIT")
                mArtistTranslit = value;
            else if (key == "#GENRE")
                mGenre = value;
            else if (key == "#CREDIT")
                mCredit = value;
            else if (key == "#BANNER")
                mBannerPath = value;
            else if (key == "#BACKGROUND")
                mBackgroundPath = value;
            else if (key == "#CDTITLE")
                mCDTitlePath = value;
            else if (key == "#MUSIC")
                mSongPath = value;
            else if (key == "#OFFSET")
                mOffset = std::stof(value);
            else if (key == "#SAMPLESTART")
                mSampleStart = std::stof(value);
            else if (key == "#SAMPLELENGTH")
                mSampleLength = std::stof(value);
            else if (key == "#SELECTABLE")
                mSelectable = value == "YES";
            else if (key == "#BPMS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float bpm = std::stof(part.substr(equalPos + 1));

                        mBPMs[measurePos] = bpm;
                    }
                }
            }
            else if (key == "#STOPS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float stopTime = std::stof(part.substr(equalPos + 1));

                        mStops[measurePos] = stopTime;
                    }
                }
            }
            else if (key == "#SPEEDS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        float speedVal = 1.0f;
                        float speedTime = 1.0f;
                        bool isBeatsOrSeconds = false;

                        part = part.substr(equalPos + 1);
                        equalPos = part.find('=');
                        if (equalPos != std::string::npos)
                        {
                            speedVal = std::stof(part.substr(0, equalPos));
                            part = part.substr(equalPos + 1);
                            equalPos = part.find('=');
                            if (equalPos != std::string::npos)
                            {
                                speedTime = std::stof(part.substr(0, equalPos));
                                isBeatsOrSeconds = part.substr(equalPos + 1) != "0";
                            }
                        }

                        mSpeeds[measurePos] = { speedVal, speedTime, isBeatsOrSeconds };
                    }
                }
            }
            else if (key == "#SCROLLS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float scrollSpeed = std::stof(part.substr(equalPos + 1));

                        mScrolls[measurePos] = scrollSpeed;
                    }
                }
            }
            else if (key == "#TICKCOUNTS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const int tickCount = std::stoi(part.substr(equalPos + 1));

                        mTickCounts[measurePos] = tickCount;
                    }
                }
            }
            else if (key == "#TIMESIGNATURES")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        part = part.substr(equalPos + 1);

                        equalPos = part.find('=');
                        if (equalPos != std::string::npos)
                        {
                            mTimeSignatures[measurePos] = { std::stoi(part.substr(0, equalPos)),
                                                            std::stoi(part.substr(equalPos + 1)) };
                        }
                    }
                }
            }
            else if (key == "#LABELS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const std::string label = part.substr(equalPos + 1);

                        mLabels[measurePos] = label;
                    }
                }
            }
            else if (key == "#COMBOS")
            {
                // @TODO
            }
            else if (key == "#DISPLAYBPM")
            {
                mDisplayBPM = std::stof(value);
            }
            else if (key == "#ORIGIN")
            {
                // @TODO
            }
            else if (key == "#PREVIEWVID")
            {
                // @TODO
            }
            else if (key == "#JACKET")
            {
                // @TODO
            }
            else if (key == "#CDIMAGE")
            {
                // @TODO
            }
            else if (key == "#DISCIMAGE")
            {
                // @TODO
            }
            else if (key == "#BGCHANGES")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const std::string label = part.substr(equalPos + 1);

                        mBGChanges[measurePos] = label;
                    }
                }
            }
            else if (key == "#FGCHANGES")
            {
                // @TODO
            }
            else if (key == "#NOTEDATA")
            {
                if (Chart* newChart = ProcessSSCChart(file))
                    mCharts[newChart->mDifficultyCategory] = newChart;
            }
        }
    }
}

Chart* Song::ProcessSMChart(std::istringstream& file)
{
    auto charCheckLambda = [](unsigned char c) { return std::isspace(c) || c == ':'; };
    auto charEraseLambda = [charCheckLambda](std::string& str) { str.erase(std::remove_if(str.begin(), str.end(), charCheckLambda), str.end()); };

    std::string danceCategory;
    std::getline(file, danceCategory); charEraseLambda(danceCategory);
    // @TODO: Double charts.
    if (danceCategory == "dance-double")
        return nullptr;

    std::string stepArtist;
    std::getline(file, stepArtist); charEraseLambda(stepArtist);
    std::string difficulty;
    std::getline(file, difficulty); charEraseLambda(difficulty);
    std::string difficultyVal;
    std::getline(file, difficultyVal); charEraseLambda(difficultyVal);
    std::string somethingVal;
    std::getline(file, somethingVal);

    Chart* currChart = new Chart(stepArtist, difficulty, std::atoi(difficultyVal.c_str()));
    currChart->ProcessNotes(file);
    return currChart;
}

Chart* Song::ProcessSSCChart(std::istringstream& file)
{
    std::string stepType, radarvalues;
    Chart* currChart = new Chart();

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '/') // Ignore empty lines and comment lines
            continue;

        // Extract the key and value from each line
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            bool isEndOfCommand = false;
            if (value.find(';') != std::string::npos)
            {
                isEndOfCommand = true;
                value.pop_back();
            }

            if (key == "#STEPSTYPE")
            {
                // TODO: dance-single
                stepType = value;
            }
            else if (key == "#DESCRIPTION")
                currChart->mStepArtist = value;
            else if (key == "#DIFFICULTY")
                currChart->mDifficultyCategory = GameUtils::StrToChartDifficulty(value);
            else if (key == "#METER")
                currChart->mDifficultyVal = std::atoi(value.c_str());
            else if (key == "#RADARVALUES")
                radarvalues = value;
            else if (key == "#NOTES")
            {
                if (stepType == "dance-double")
                    return nullptr;
                    
                mChartDifficulties[currChart->mDifficultyCategory] = currChart->mDifficultyVal;
                currChart->ProcessNotes(file);
                return currChart;
            }
        }
    }

    return nullptr;
}

void Song::ProcessSMD(std::istringstream& file)
{
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '/') // Ignore empty lines and comment lines
            continue;

        // Extract the key and value from each line
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            bool isEndOfCommand = false;
            if (value.find(';') != std::string::npos)
            {
                isEndOfCommand = true;
                value.pop_back();
            }

            // Store the value in the appropriate member of the SongData struct
            if (key == "#TITLE")
                mTitle = value;
            else if (key == "#SUBTITLE")
                mSubtitle = value;
            else if (key == "#ARTIST")
                mArtist = value;
            else if (key == "#TITLETRANSLIT")
                mTitleTranslit = value;
            else if (key == "#SUBTITLETRANSLIT")
                mSubtitleTranslit = value;
            else if (key == "#ARTISTTRANSLIT")
                mArtistTranslit = value;
            else if (key == "#GENRE")
                mGenre = value;
            else if (key == "#CREDIT")
                mCredit = value;
            else if (key == "#BANNER")
                mBannerPath = value;
            else if (key == "#BACKGROUND")
                mBackgroundPath = value;
            else if (key == "#CDTITLE")
                mCDTitlePath = value;
            else if (key == "#MUSIC")
                mSongPath = value;
            else if (key == "#OFFSET")
                mOffset = std::stof(value);
            else if (key == "#SAMPLESTART")
                mSampleStart = std::stof(value);
            else if (key == "#SAMPLELENGTH")
                mSampleLength = std::stof(value);
            else if (key == "#SELECTABLE")
                mSelectable = value == "YES";
            else if (key == "#BPMS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float bpm = std::stof(part.substr(equalPos + 1));

                        mBPMs[measurePos] = bpm;
                    }
                }
            }
            else if (key == "#STOPS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);

                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float stopTime = std::stof(part.substr(equalPos + 1));

                        mStops[measurePos] = stopTime;
                    }
                }
            }
            else if (key == "#SPEEDS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        float speedVal = 1.0f;
                        float speedTime = 1.0f;
                        bool isBeatsOrSeconds = false;

                        part = part.substr(equalPos + 1);
                        equalPos = part.find('=');
                        if (equalPos != std::string::npos)
                        {
                            speedVal = std::stof(part.substr(0, equalPos));
                            part = part.substr(equalPos + 1);
                            equalPos = part.find('=');
                            if (equalPos != std::string::npos)
                            {
                                speedTime = std::stof(part.substr(0, equalPos));
                                isBeatsOrSeconds = part.substr(equalPos + 1) != "0";
                            }
                        }

                        mSpeeds[measurePos] = { speedVal, speedTime, isBeatsOrSeconds };
                    }
                }
            }
            else if (key == "#SCROLLS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const float scrollSpeed = std::stof(part.substr(equalPos + 1));

                        mScrolls[measurePos] = scrollSpeed;
                    }
                }
            }
            else if (key == "#TICKCOUNTS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const int tickCount = std::stoi(part.substr(equalPos + 1));

                        mTickCounts[measurePos] = tickCount;
                    }
                }
            }
            else if (key == "#TIMESIGNATURES")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        part = part.substr(equalPos + 1);

                        equalPos = part.find('=');
                        if (equalPos != std::string::npos)
                        {
                            mTimeSignatures[measurePos] = { std::stoi(part.substr(0, equalPos)),
                                                            std::stoi(part.substr(equalPos + 1)) };
                        }
                    }
                }
            }
            else if (key == "#LABELS")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const std::string label = part.substr(equalPos + 1);

                        mLabels[measurePos] = label;
                    }
                }
            }
            else if (key == "#COMBOS")
            {
                // @TODO
            }
            else if (key == "#DISPLAYBPM")
            {
                if (value != "*")
                    mDisplayBPM = std::stof(value);
            }
            else if (key == "#ORIGIN")
            {
                // @TODO
            }
            else if (key == "#PREVIEWVID")
            {
                // @TODO
            }
            else if (key == "#JACKET")
            {
                // @TODO
            }
            else if (key == "#CDIMAGE")
            {
                // @TODO
            }
            else if (key == "#DISCIMAGE")
            {
                // @TODO
            }
            else if (key == "#BGCHANGES")
            {
                while (!isEndOfCommand)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEndOfCommand |= currLine.find(';') != std::string::npos;

                    if (isEndOfCommand)
                        value.pop_back();
                }

                std::stringstream ss(value);
                std::string part;

                while (std::getline(ss, part, ','))
                {
                    size_t equalPos = part.find('=');
                    if (equalPos != std::string::npos)
                    {
                        const float measurePos = std::stof(part.substr(0, equalPos));
                        const std::string label = part.substr(equalPos + 1);

                        mBGChanges[measurePos] = label;
                    }
                }
            }
            else if (key == "#FGCHANGES")
            {
                // @TODO
            }
            else if (key == "#CHARTS")
            {
                while (!isEndOfCommand)
                {
                    std::getline(file, line);
                    colonPos = line.find(':');
                    key = line.substr(0, colonPos);
                    value = line.substr(colonPos + 1);

                    isEndOfCommand |= line.find(';') != std::string::npos;
                    if (isEndOfCommand)
                    {
                        value.pop_back();
                        break;
                    }

                    mChartDifficulties[GameUtils::StrToChartDifficulty(key)] = std::stoi(value);
                }
            }
        }
    }
}

void Song::ProcessSCD(std::istringstream& file)
{

    std::string line;
    while (std::getline(file, line))
    {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;

        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        if (key == "#NEWCHART")
        {
            if (Chart* newChart = ProcessSSCChart(file))
                mCharts[newChart->mDifficultyCategory] = newChart;
        }
    }
}

Chart* Song::ProcessSCDChart(std::istringstream& file)
{
    std::string stepType, radarvalues;
    Chart* currChart = new Chart();

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '/') // Ignore empty lines and comment lines
            continue;

        // Extract the key and value from each line
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            if (key == "#STEPSTYPE")
            {
                // TODO: dance-single
                stepType = value;
            }
            else if (key == "#AUTHOR")
                currChart->mStepArtist = value;
            else if (key == "#DIFFICULTY")
                currChart->mDifficultyCategory = GameUtils::StrToChartDifficulty(value);
            else if (key == "#METER")
                currChart->mDifficultyVal = std::atoi(value.c_str());
            else if (key == "#RADARVALUES")
                radarvalues = value;
            else if (key == "#NOTES")
            {
                currChart->ProcessNotes(file);
                return currChart;
            }
        }
    }

    return nullptr;
}

void Song::SaveAsSSCSong(const std::string& outPath)
{
    const std::string path = outPath.empty() ? mPath + mTitle + ".ssc" : ResourceMgr->GetPathWithoutExtension(outPath) + ".ssc";
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), path);

    file << "#VERSION:0.83;" << std::endl;
    file << "#TITLE:" << mTitle << ";" << std::endl;
    file << "#SUBTITLE:" << mSubtitle << ";" << std::endl;
    file << "#ARTIST:" << mArtist << ";" << std::endl;
    file << "#TITLETRANSLIT:" << mTitleTranslit << ";" << std::endl;
    file << "#SUBTITLETRANSLIT:" << mSubtitleTranslit << ";" << std::endl;
    file << "#ARTISTTRANSLIT:" << mArtistTranslit << ";" << std::endl;
    file << "#GENRE:" << mGenre << ";" << std::endl;
    file << "#CREDIT:" << mCredit << ";" << std::endl;
    file << "#MUSIC:" << mSongPath << ";" << std::endl;
    file << "#BANNER:" << mBannerPath << ";" << std::endl;
    file << "#BACKGROUND:" << mBackgroundPath << ";" << std::endl;
    file << "#CDTITLE:" << mCDTitlePath << ";" << std::endl;
    file << "#SAMPLESTART:" << mSampleStart << ";" << std::endl;
    file << "#SAMPLELENGTH:" << mSampleLength << ";" << std::endl;
    const std::string selectableStr = mSelectable ? "YES" : "NO";
    file << "#SELECTABLE:" << selectableStr << ";" << std::endl;
    file << "#OFFSET:" << mOffset << ";" << std::endl;

    // BPMs
    file << std::fixed << std::setprecision(3);
    file << "#BPMS:";
    for (auto it = mBPMs.begin(); it != mBPMs.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mBPMs.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Stops
    file << "#STOPS:";
    for (auto it = mStops.begin(); it != mStops.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mStops.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Speeds
    file << "#SPEEDS:";
    for (auto it = mSpeeds.begin(); it != mSpeeds.end(); ++it)
    {
        file << it->first << "=" << std::get<0>(it->second) << "=" << std::get<1>(it->second) << "=" << std::get<2>(it->second) << std::endl;
        if (std::next(it) != mSpeeds.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#SCROLLS:";
    for (auto it = mScrolls.begin(); it != mScrolls.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mScrolls.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TICKCOUNTS:";
    for (auto it = mTickCounts.begin(); it != mTickCounts.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mTickCounts.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TIMESIGNATURES:";
    for (auto it = mTimeSignatures.begin(); it != mTimeSignatures.end(); ++it)
    {
        file << it->first << "=" << it->second.first << "=" << it->second.second << std::endl;
        if (std::next(it) != mTimeSignatures.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#LABELS:";
    for (auto it = mLabels.begin(); it != mLabels.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mLabels.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#COMBOS:";
    for (auto it = mCombos.begin(); it != mCombos.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mCombos.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#DISPLAYBPM:" << mDisplayBPM << ";" << std::endl;
    file << "#ORIGIN:" << mOrigin << ";" << std::endl;
    file << "#PREVIEWVID:" << mPreviewVID << ";" << std::endl;
    file << "#JACKET:" << mJacket << ";" << std::endl;
    file << "#CDIMAGE:" << mCDImage << ";" << std::endl;
    file << "#DISCIMAGE:" << mDiscImage << ";" << std::endl;

    file << "#BGCHANGES:";
    for (auto it = mBGChanges.begin(); it != mBGChanges.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mBGChanges.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#FGCHANGES:" << mFGChanges << ";" << std::endl;

    for (auto chart : mCharts)
    {
        Chart* chartData = chart.second;
        file << "//--------------- dance-single - " << chartData->mStepArtist << " ----------------" << std::endl;
        file << "#NOTEDATA:" << "" << ";" << std::endl; // TODO
        file << "#STEPSTYPE:" << "dance-single" << ";" << std::endl; // TODO
        file << "#DESCRIPTION:" << chartData->mStepArtist << ";" << std::endl;

        file << "#DIFFICULTY:";
        file << GameUtils::ChartDifficultyToStr(chartData->mDifficultyCategory) << ";" << std::endl;

        file << "#METER:" << chartData->mDifficultyVal << ";" << std::endl;
        file << "#RADARVALUES:" << "0,0,0,0,0" << ";" << std::endl; // TODO

        file << "#NOTES:" << std::endl;
        chartData->SaveNotes(file);
    }

    file.close();

    GlobalEvents::gOnSongCreate.Broadcast(this);
}

void Song::SaveToSMD(const std::string& outPath)
{
    const std::string path = outPath.empty() ? mPath + mTitle + ".smd" : ResourceMgr->GetPathWithoutExtension(outPath) + ".smd";
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), " " + path);

    file << "#VERSION:" << gGameVariables.mMajorVersion << "." << gGameVariables.mMinorVersion << "."
         << gGameVariables.mPatchVersion << ";" << std::endl;
    file << "#TITLE:" << mTitle << ";" << std::endl;
    file << "#SUBTITLE:" << mSubtitle << ";" << std::endl;
    file << "#ARTIST:" << mArtist << ";" << std::endl;
    file << "#TITLETRANSLIT:" << mTitleTranslit << ";" << std::endl;
    file << "#SUBTITLETRANSLIT:" << mSubtitleTranslit << ";" << std::endl;
    file << "#ARTISTTRANSLIT:" << mArtistTranslit << ";" << std::endl;
    file << "#GENRE:" << mGenre << ";" << std::endl;
    file << "#CREDIT:" << mCredit << ";" << std::endl;
    file << "#MUSIC:" << mSongPath << ";" << std::endl;
    file << "#BANNER:" << mBannerPath << ";" << std::endl;
    file << "#BACKGROUND:" << mBackgroundPath << ";" << std::endl;
    file << "#CDTITLE:" << mCDTitlePath << ";" << std::endl;
    file << "#SAMPLESTART:" << mSampleStart << ";" << std::endl;
    file << "#SAMPLELENGTH:" << mSampleLength << ";" << std::endl;
    const std::string selectableStr = mSelectable ? "YES" : "NO";
    file << "#SELECTABLE:" << selectableStr << ";" << std::endl;
    file << "#OFFSET:" << mOffset << ";" << std::endl;

    // BPMs
    file << std::fixed << std::setprecision(3);
    file << "#BPMS:";
    for (auto it = mBPMs.begin(); it != mBPMs.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mBPMs.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Stops
    file << "#STOPS:";
    for (auto it = mStops.begin(); it != mStops.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mStops.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Speeds
    file << "#SPEEDS:";
    for (auto it = mSpeeds.begin(); it != mSpeeds.end(); ++it)
    {
        file << it->first << "=" << std::get<0>(it->second) << "=" << std::get<1>(it->second) << "=" << std::get<2>(it->second) << std::endl;
        if (std::next(it) != mSpeeds.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#SCROLLS:";
    for (auto it = mScrolls.begin(); it != mScrolls.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mScrolls.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TICKCOUNTS:";
    for (auto it = mTickCounts.begin(); it != mTickCounts.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mTickCounts.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TIMESIGNATURES:";
    for (auto it = mTimeSignatures.begin(); it != mTimeSignatures.end(); ++it)
    {
        file << it->first << "=" << it->second.first << "=" << it->second.second << std::endl;
        if (std::next(it) != mTimeSignatures.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#LABELS:";
    for (auto it = mLabels.begin(); it != mLabels.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mLabels.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#COMBOS:";
    for (auto it = mCombos.begin(); it != mCombos.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mCombos.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#DISPLAYBPM:" << mDisplayBPM << ";" << std::endl;
    file << "#ORIGIN:" << mOrigin << ";" << std::endl;
    file << "#PREVIEWVID:" << mPreviewVID << ";" << std::endl;
    file << "#JACKET:" << mJacket << ";" << std::endl;
    file << "#CDIMAGE:" << mCDImage << ";" << std::endl;
    file << "#DISCIMAGE:" << mDiscImage << ";" << std::endl;

    file << "#BGCHANGES:";
    for (auto it = mBGChanges.begin(); it != mBGChanges.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mBGChanges.end())
            file << ",";
    }
    file << ";" << std::endl;

    // In front of the UI
    file << "#FGCHANGES:" << mFGChanges << ";" << std::endl;

    file << "#CHARTS:" << std::endl;
    for (auto chart : mCharts)
    {
        Chart* chartData = chart.second;
        file << GameUtils::ChartDifficultyToStr(chartData->mDifficultyCategory) << ":" << chartData->mDifficultyVal << std::endl;
    }
    file << ";" << std::endl;

    file.close();
}

void Song::SaveToSCD(const std::string& outPath)
{
    const std::string path = outPath.empty() ? mPath + mTitle + ".scd" : ResourceMgr->GetPathWithoutExtension(outPath) + ".scd";
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), " " + path);

    for (auto chart : mCharts)
    {
        Chart* chartData = chart.second;
        file << "//--------------- dance-single - " << chartData->mStepArtist << " ----------------" << std::endl;
        file << "#NEWCHART:" << "" << ";" << std::endl; // TODO
        file << "#STEPSTYPE:" << "dance-single" << ";" << std::endl; // TODO
        file << "#AUTHOR:" << chartData->mStepArtist << ";" << std::endl;

        file << "#DIFFICULTY:";
        file << GameUtils::ChartDifficultyToStr(chartData->mDifficultyCategory) << ";" << std::endl;

        file << "#METER:" << chartData->mDifficultyVal << ";" << std::endl;
        file << "#RADARVALUES:" << "0,0,0,0,0" << ";" << std::endl; // TODO

        file << "#NOTES:" << std::endl;
        chartData->SaveNotes(file);
    }

    file.close();
}
