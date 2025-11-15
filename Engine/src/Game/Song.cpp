#include "Song.h"
#include "Audio/Audio.h"
#include "GlobalEvents.h"
#include "GlobalVariables.h"
#include "Graphics/GfxMgr.h"
#include "Misc/Requires.h"
#include "Utils/GameUtils.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

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

void RemoveCarriageReturns(std::string& text) { text.erase(std::remove(text.begin(), text.end(), '\r'), text.end()); }

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

    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    RemoveContents(fileContents);
    std::istringstream cleanContents(fileContents);

    std::string extension = ResourceMgr->GetExtension(path);
    if (extension == "sm")
        ProcessSMSong(cleanContents);
    else if (extension == "ssc")
        ProcessSSCSong(cleanContents);
    else if (extension == "smd")
        ProcessSMD(cleanContents);
    else
        PrintError("Unknown extension for song: {}", mPath);

    file.close();

    GlobalEvents::gOnSongCreate.Broadcast(this);
}

void Song::Play(const float measure)
{
    if (measure >= mSongInfo.mOffset)
    {
        mSong->get()->Play();
        mSong->get()->SetPosition(measure - mSongInfo.mOffset);
        GlobalEvents::gOnSongPlay.Broadcast(this);
    }
    mIsPlaying = true;
}

#undef max
float Song::GetBPMAt(const float time)
{
    switch (mSongInfo.mBPMs.size())
    {
    case 0:
        return std::numeric_limits<float>::max();
    case 1:
        return mSongInfo.mBPMs.begin()->second;
    default:
        auto it = mSongInfo.mBPMs.begin();
        std::advance(it, 1);

        for (; it != mSongInfo.mBPMs.end(); ++it)
        {
            if (time < it->first)
            {
                auto prevIt = --it;
                return prevIt->second;
            }
        }

        return mSongInfo.mBPMs.rbegin()->second;
    }
}

Resource<Audio>* Song::GetSong()
{
    if (mSong)
        return mSong;

    mSong = !mSongInfo.mSongPath.empty() ? ResourceMgr->Load<Audio>(mPath + mSongInfo.mSongPath)
                                         : ResourceMgr->GetDefaultAsset<Audio>();

    return mSong;
}

Resource<Texture>* Song::GetBanner()
{
    if (mBanner)
        return mBanner;

    mBanner = !mSongInfo.mBannerPath.empty() ? ResourceMgr->Load<Texture>(mPath + mSongInfo.mBannerPath)
                                             : ResourceMgr->GetDefaultAsset<Texture>();

    return mBanner;
}

Resource<Texture>* Song::GetBackground()
{
    if (mBackground)
        return mBackground;

    mBackground = !mSongInfo.mBackgroundPath.empty() ? ResourceMgr->Load<Texture>(mPath + mSongInfo.mBackgroundPath)
                                                     : ResourceMgr->GetDefaultAsset<Texture>();

    return mBackground;
}

Resource<Texture>* Song::GetCDTitle()
{
    if (mCDTitle)
        return mCDTitle;

    mCDTitle = !mSongInfo.mCDTitlePath.empty() ? ResourceMgr->Load<Texture>(mPath + mSongInfo.mCDTitlePath)
                                               : ResourceMgr->GetDefaultAsset<Texture>();

    return mCDTitle;
}

void Song::ProcessSMSong(std::istringstream& file)
{
    mFileType = FileType::SM;
    std::streampos lastPos = file.tellg();

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
                mSongInfo.mTitle = value;
            else if (key == "#SUBTITLE")
                mSongInfo.mSubtitle = value;
            else if (key == "#ARTIST")
                mSongInfo.mArtist = value;
            else if (key == "#TITLETRANSLIT")
                mSongInfo.mTitleTranslit = value;
            else if (key == "#SUBTITLETRANSLIT")
                mSongInfo.mSubtitleTranslit = value;
            else if (key == "#ARTISTTRANSLIT")
                mSongInfo.mArtistTranslit = value;
            else if (key == "#GENRE")
                mSongInfo.mGenre = value;
            else if (key == "#CREDIT")
                mSongInfo.mCredit = value;
            else if (key == "#BANNER")
                mSongInfo.mBannerPath = value;
            else if (key == "#BACKGROUND")
                mSongInfo.mBackgroundPath = value;
            else if (key == "#CDTITLE")
                mSongInfo.mCDTitlePath = value;
            else if (key == "#MUSIC")
                mSongInfo.mSongPath = value;
            else if (key == "#OFFSET")
                mSongInfo.mOffset = std::stof(value);
            else if (key == "#SAMPLESTART")
                mSongInfo.mSampleStart = std::stof(value);
            else if (key == "#SAMPLELENGTH")
                mSongInfo.mSampleLength = std::stof(value);
            else if (key == "#SELECTABLE")
                mSongInfo.mSelectable = value == "YES";
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

                        mSongInfo.mBPMs[measurePos] = bpm;
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

                        mSongInfo.mStops[measurePos] = stopTime;
                    }
                }
            }
            else if (key == "#NOTES")
            {
                file.clear();
                file.seekg(lastPos);

                if (Chart* newChart = ProcessSMChart(file))
                    mCharts[newChart->mDifficultyCategory] = newChart;
            }
        }

        lastPos = file.tellg();
    }
}

void Song::ProcessSSCSong(std::istringstream& file)
{
    mFileType = FileType::SSC;
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
                mSongInfo.mTitle = value;
            else if (key == "#SUBTITLE")
                mSongInfo.mSubtitle = value;
            else if (key == "#ARTIST")
                mSongInfo.mArtist = value;
            else if (key == "#TITLETRANSLIT")
                mSongInfo.mTitleTranslit = value;
            else if (key == "#SUBTITLETRANSLIT")
                mSongInfo.mSubtitleTranslit = value;
            else if (key == "#ARTISTTRANSLIT")
                mSongInfo.mArtistTranslit = value;
            else if (key == "#GENRE")
                mSongInfo.mGenre = value;
            else if (key == "#CREDIT")
                mSongInfo.mCredit = value;
            else if (key == "#BANNER")
                mSongInfo.mBannerPath = value;
            else if (key == "#BACKGROUND")
                mSongInfo.mBackgroundPath = value;
            else if (key == "#CDTITLE")
                mSongInfo.mCDTitlePath = value;
            else if (key == "#MUSIC")
                mSongInfo.mSongPath = value;
            else if (key == "#OFFSET")
                mSongInfo.mOffset = std::stof(value);
            else if (key == "#SAMPLESTART")
                mSongInfo.mSampleStart = std::stof(value);
            else if (key == "#SAMPLELENGTH")
                mSongInfo.mSampleLength = std::stof(value);
            else if (key == "#SELECTABLE")
                mSongInfo.mSelectable = value == "YES";
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

                        mSongInfo.mBPMs[measurePos] = bpm;
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

                        mSongInfo.mStops[measurePos] = stopTime;
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

                        mSongInfo.mSpeeds[measurePos] = {speedVal, speedTime, isBeatsOrSeconds};
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

                        mSongInfo.mScrolls[measurePos] = scrollSpeed;
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

                        mSongInfo.mTickCounts[measurePos] = tickCount;
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
                            mSongInfo.mTimeSignatures[measurePos] = {std::stoi(part.substr(0, equalPos)),
                                                                     std::stoi(part.substr(equalPos + 1))};
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

                        mSongInfo.mLabels[measurePos] = label;
                    }
                }
            }
            else if (key == "#COMBOS")
            {
                // @TODO
            }
            else if (key == "#DISPLAYBPM")
            {
                mSongInfo.mDisplayBPM = std::stof(value);
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

                        mSongInfo.mBGChanges[measurePos] = label;
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
    auto charEraseLambda = [charCheckLambda](std::string& str)
    { str.erase(std::remove_if(str.begin(), str.end(), charCheckLambda), str.end()); };

    std::string notesLabel;
    std::getline(file, notesLabel, ':');

    std::string danceCategory;
    std::getline(file, danceCategory, ':');
    charEraseLambda(danceCategory);
    // @TODO: Double charts.
    if (danceCategory == "dance-double")
        return nullptr;

    std::string stepArtist;
    std::getline(file, stepArtist, ':');
    charEraseLambda(stepArtist);
    std::string difficulty;
    std::getline(file, difficulty, ':');
    charEraseLambda(difficulty);
    std::string difficultyVal;
    std::getline(file, difficultyVal, ':');
    charEraseLambda(difficultyVal);
    std::string somethingVal;
    std::getline(file, somethingVal, ':');

    Chart* currChart = new Chart(stepArtist, difficulty, std::atoi(difficultyVal.c_str()));
    currChart->ProcessNotes(file);
    currChart->ProcessTimestamps(mSongInfo.mBPMs, mSongInfo.mStops, mSongInfo.mOffset);
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
                currChart->ProcessTimestamps(mSongInfo.mBPMs, mSongInfo.mStops, mSongInfo.mOffset);
                return currChart;
            }
        }
    }

    return nullptr;
}

void Song::ProcessSMD(std::istringstream& file)
{
    mFileType = FileType::SMD_SCD;
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
                mSongInfo.mTitle = value;
            else if (key == "#SUBTITLE")
                mSongInfo.mSubtitle = value;
            else if (key == "#ARTIST")
                mSongInfo.mArtist = value;
            else if (key == "#TITLETRANSLIT")
                mSongInfo.mTitleTranslit = value;
            else if (key == "#SUBTITLETRANSLIT")
                mSongInfo.mSubtitleTranslit = value;
            else if (key == "#ARTISTTRANSLIT")
                mSongInfo.mArtistTranslit = value;
            else if (key == "#GENRE")
                mSongInfo.mGenre = value;
            else if (key == "#CREDIT")
                mSongInfo.mCredit = value;
            else if (key == "#BANNER")
                mSongInfo.mBannerPath = value;
            else if (key == "#BACKGROUND")
                mSongInfo.mBackgroundPath = value;
            else if (key == "#CDTITLE")
                mSongInfo.mCDTitlePath = value;
            else if (key == "#MUSIC")
                mSongInfo.mSongPath = value;
            else if (key == "#OFFSET")
                mSongInfo.mOffset = std::stof(value);
            else if (key == "#SAMPLESTART")
                mSongInfo.mSampleStart = std::stof(value);
            else if (key == "#SAMPLELENGTH")
                mSongInfo.mSampleLength = std::stof(value);
            else if (key == "#SELECTABLE")
                mSongInfo.mSelectable = value == "YES";
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

                        mSongInfo.mBPMs[measurePos] = bpm;
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

                        mSongInfo.mStops[measurePos] = stopTime;
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

                        mSongInfo.mSpeeds[measurePos] = {speedVal, speedTime, isBeatsOrSeconds};
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

                        mSongInfo.mScrolls[measurePos] = scrollSpeed;
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

                        mSongInfo.mTickCounts[measurePos] = tickCount;
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
                            mSongInfo.mTimeSignatures[measurePos] = {std::stoi(part.substr(0, equalPos)),
                                                                     std::stoi(part.substr(equalPos + 1))};
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

                        mSongInfo.mLabels[measurePos] = label;
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
                    mSongInfo.mDisplayBPM = std::stof(value);
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

                        mSongInfo.mBGChanges[measurePos] = label;
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
                currChart->ProcessTimestamps(mSongInfo.mBPMs, mSongInfo.mStops, mSongInfo.mOffset);
                return currChart;
            }
        }
    }

    return nullptr;
}

void Song::SaveAsSSCSong(const std::string& outPath)
{
    const std::string path =
        outPath.empty() ? mPath + mSongInfo.mTitle + ".ssc" : ResourceMgr->GetPathWithoutExtension(outPath) + ".ssc";
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), path);

    file << "#VERSION:0.83;" << std::endl;
    file << "#TITLE:" << mSongInfo.mTitle << ";" << std::endl;
    file << "#SUBTITLE:" << mSongInfo.mSubtitle << ";" << std::endl;
    file << "#ARTIST:" << mSongInfo.mArtist << ";" << std::endl;
    file << "#TITLETRANSLIT:" << mSongInfo.mTitleTranslit << ";" << std::endl;
    file << "#SUBTITLETRANSLIT:" << mSongInfo.mSubtitleTranslit << ";" << std::endl;
    file << "#ARTISTTRANSLIT:" << mSongInfo.mArtistTranslit << ";" << std::endl;
    file << "#GENRE:" << mSongInfo.mGenre << ";" << std::endl;
    file << "#CREDIT:" << mSongInfo.mCredit << ";" << std::endl;
    file << "#MUSIC:" << mSongInfo.mSongPath << ";" << std::endl;
    file << "#BANNER:" << mSongInfo.mBannerPath << ";" << std::endl;
    file << "#BACKGROUND:" << mSongInfo.mBackgroundPath << ";" << std::endl;
    file << "#CDTITLE:" << mSongInfo.mCDTitlePath << ";" << std::endl;
    file << "#SAMPLESTART:" << mSongInfo.mSampleStart << ";" << std::endl;
    file << "#SAMPLELENGTH:" << mSongInfo.mSampleLength << ";" << std::endl;
    const std::string selectableStr = mSongInfo.mSelectable ? "YES" : "NO";
    file << "#SELECTABLE:" << selectableStr << ";" << std::endl;
    file << "#OFFSET:" << mSongInfo.mOffset << ";" << std::endl;

    // BPMs
    file << std::fixed << std::setprecision(3);
    file << "#BPMS:";
    for (auto it = mSongInfo.mBPMs.begin(); it != mSongInfo.mBPMs.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mBPMs.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Stops
    file << "#STOPS:";
    for (auto it = mSongInfo.mStops.begin(); it != mSongInfo.mStops.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mStops.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Speeds
    file << "#SPEEDS:";
    for (auto it = mSongInfo.mSpeeds.begin(); it != mSongInfo.mSpeeds.end(); ++it)
    {
        file << it->first << "=" << std::get<0>(it->second) << "=" << std::get<1>(it->second) << "="
             << std::get<2>(it->second) << std::endl;
        if (std::next(it) != mSongInfo.mSpeeds.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#SCROLLS:";
    for (auto it = mSongInfo.mScrolls.begin(); it != mSongInfo.mScrolls.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mScrolls.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TICKCOUNTS:";
    for (auto it = mSongInfo.mTickCounts.begin(); it != mSongInfo.mTickCounts.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mTickCounts.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TIMESIGNATURES:";
    for (auto it = mSongInfo.mTimeSignatures.begin(); it != mSongInfo.mTimeSignatures.end(); ++it)
    {
        file << it->first << "=" << it->second.first << "=" << it->second.second << std::endl;
        if (std::next(it) != mSongInfo.mTimeSignatures.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#LABELS:";
    for (auto it = mSongInfo.mLabels.begin(); it != mSongInfo.mLabels.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mLabels.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#COMBOS:";
    for (auto it = mSongInfo.mCombos.begin(); it != mSongInfo.mCombos.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mCombos.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#DISPLAYBPM:" << mSongInfo.mDisplayBPM << ";" << std::endl;
    file << "#ORIGIN:" << mSongInfo.mOrigin << ";" << std::endl;
    file << "#PREVIEWVID:" << mSongInfo.mPreviewVID << ";" << std::endl;
    file << "#JACKET:" << mSongInfo.mJacket << ";" << std::endl;
    file << "#CDIMAGE:" << mSongInfo.mCDImage << ";" << std::endl;
    file << "#DISCIMAGE:" << mSongInfo.mDiscImage << ";" << std::endl;

    file << "#BGCHANGES:";
    for (auto it = mSongInfo.mBGChanges.begin(); it != mSongInfo.mBGChanges.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mBGChanges.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#FGCHANGES:" << mSongInfo.mFGChanges << ";" << std::endl;

    for (auto chart : mCharts)
    {
        Chart* chartData = chart.second;
        file << "//--------------- dance-single - " << chartData->mStepArtist << " ----------------" << std::endl;
        file << "#NOTEDATA:" << "" << ";" << std::endl;              // TODO
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
    const std::string path =
        outPath.empty() ? mPath + mSongInfo.mTitle + ".smd" : ResourceMgr->GetPathWithoutExtension(outPath) + ".smd";
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), " " + path);

    file << "#VERSION:" << gGlobalVariables.mMajorVersion << "." << gGlobalVariables.mMinorVersion << "."
         << gGlobalVariables.mPatchVersion << ";" << std::endl;
    file << "#TITLE:" << mSongInfo.mTitle << ";" << std::endl;
    file << "#SUBTITLE:" << mSongInfo.mSubtitle << ";" << std::endl;
    file << "#ARTIST:" << mSongInfo.mArtist << ";" << std::endl;
    file << "#TITLETRANSLIT:" << mSongInfo.mTitleTranslit << ";" << std::endl;
    file << "#SUBTITLETRANSLIT:" << mSongInfo.mSubtitleTranslit << ";" << std::endl;
    file << "#ARTISTTRANSLIT:" << mSongInfo.mArtistTranslit << ";" << std::endl;
    file << "#GENRE:" << mSongInfo.mGenre << ";" << std::endl;
    file << "#CREDIT:" << mSongInfo.mCredit << ";" << std::endl;
    file << "#MUSIC:" << mSongInfo.mSongPath << ";" << std::endl;
    file << "#BANNER:" << mSongInfo.mBannerPath << ";" << std::endl;
    file << "#BACKGROUND:" << mSongInfo.mBackgroundPath << ";" << std::endl;
    file << "#CDTITLE:" << mSongInfo.mCDTitlePath << ";" << std::endl;
    file << "#SAMPLESTART:" << mSongInfo.mSampleStart << ";" << std::endl;
    file << "#SAMPLELENGTH:" << mSongInfo.mSampleLength << ";" << std::endl;
    const std::string selectableStr = mSongInfo.mSelectable ? "YES" : "NO";
    file << "#SELECTABLE:" << selectableStr << ";" << std::endl;
    file << "#OFFSET:" << mSongInfo.mOffset << ";" << std::endl;

    // BPMs
    file << std::fixed << std::setprecision(3);
    file << "#BPMS:";
    for (auto it = mSongInfo.mBPMs.begin(); it != mSongInfo.mBPMs.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mBPMs.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Stops
    file << "#STOPS:";
    for (auto it = mSongInfo.mStops.begin(); it != mSongInfo.mStops.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mStops.end())
            file << ",";
    }
    file << ";" << std::endl;

    // Speeds
    file << "#SPEEDS:";
    for (auto it = mSongInfo.mSpeeds.begin(); it != mSongInfo.mSpeeds.end(); ++it)
    {
        file << it->first << "=" << std::get<0>(it->second) << "=" << std::get<1>(it->second) << "="
             << std::get<2>(it->second) << std::endl;
        if (std::next(it) != mSongInfo.mSpeeds.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#SCROLLS:";
    for (auto it = mSongInfo.mScrolls.begin(); it != mSongInfo.mScrolls.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mScrolls.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TICKCOUNTS:";
    for (auto it = mSongInfo.mTickCounts.begin(); it != mSongInfo.mTickCounts.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mTickCounts.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#TIMESIGNATURES:";
    for (auto it = mSongInfo.mTimeSignatures.begin(); it != mSongInfo.mTimeSignatures.end(); ++it)
    {
        file << it->first << "=" << it->second.first << "=" << it->second.second << std::endl;
        if (std::next(it) != mSongInfo.mTimeSignatures.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#LABELS:";
    for (auto it = mSongInfo.mLabels.begin(); it != mSongInfo.mLabels.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mLabels.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#COMBOS:";
    for (auto it = mSongInfo.mCombos.begin(); it != mSongInfo.mCombos.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mCombos.end())
            file << ",";
    }
    file << ";" << std::endl;

    file << "#DISPLAYBPM:" << mSongInfo.mDisplayBPM << ";" << std::endl;
    file << "#ORIGIN:" << mSongInfo.mOrigin << ";" << std::endl;
    file << "#PREVIEWVID:" << mSongInfo.mPreviewVID << ";" << std::endl;
    file << "#JACKET:" << mSongInfo.mJacket << ";" << std::endl;
    file << "#CDIMAGE:" << mSongInfo.mCDImage << ";" << std::endl;
    file << "#DISCIMAGE:" << mSongInfo.mDiscImage << ";" << std::endl;

    file << "#BGCHANGES:";
    for (auto it = mSongInfo.mBGChanges.begin(); it != mSongInfo.mBGChanges.end(); ++it)
    {
        file << it->first << "=" << it->second << std::endl;
        if (std::next(it) != mSongInfo.mBGChanges.end())
            file << ",";
    }
    file << ";" << std::endl;

    // In front of the UI
    file << "#FGCHANGES:" << mSongInfo.mFGChanges << ";" << std::endl;

    file << "#CHARTS:" << std::endl;
    for (auto chart : mCharts)
    {
        Chart* chartData = chart.second;
        file << GameUtils::ChartDifficultyToStr(chartData->mDifficultyCategory) << ":" << chartData->mDifficultyVal
             << std::endl;
    }
    file << ";" << std::endl;

    file.close();
}

void Song::SaveToSCD(const std::string& outPath)
{
    const std::string path =
        outPath.empty() ? mPath + mSongInfo.mTitle + ".scd" : ResourceMgr->GetPathWithoutExtension(outPath) + ".scd";
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), " " + path);

    for (auto chart : mCharts)
    {
        Chart* chartData = chart.second;
        file << "//--------------- dance-single - " << chartData->mStepArtist << " ----------------" << std::endl;
        file << "#NEWCHART:" << "" << ";" << std::endl;              // @TODO
        file << "#STEPSTYPE:" << "dance-single" << ";" << std::endl; // @TODO
        file << "#AUTHOR:" << chartData->mStepArtist << ";" << std::endl;

        file << "#DIFFICULTY:";
        file << GameUtils::ChartDifficultyToStr(chartData->mDifficultyCategory) << ";" << std::endl;

        file << "#METER:" << chartData->mDifficultyVal << ";" << std::endl;
        file << "#RADARVALUES:" << "0,0,0,0,0" << ";" << std::endl; // @TODO

        file << "#NOTES:" << std::endl;
        chartData->SaveNotes(file);
    }

    file.close();
}

void Song::LoadChartsSCD()
{
    if (mFileType != FileType::SMD_SCD)
    {
        PrintError("Trying to load charts for an unsupported file extension: {}", mPath);
    }

    std::string pathSCD = FileUtils::JoinPath(mPath, mSongInfo.mTitle + ".scd");
    std::ifstream file(pathSCD);
    if (!file.is_open() || !file.good())
    {

        // try finding a scd in there
        auto firstSCD = FileUtils::FindFirstFileOf(mPath, ".scd");
        if (!firstSCD)
        {
            PrintError(".scd file not found in {}", mPath);
            return;
        }

        pathSCD = firstSCD.value();
        file = std::ifstream(pathSCD);
        if (!file.is_open() || !file.good())
        {
            PrintError("File not read in {}", pathSCD);
            return;
        }
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    RemoveContents(fileContents);
    std::istringstream cleanContents(fileContents);
    ProcessSCD(cleanContents);
}

std::vector<float> Song::GetBeatTimestamps()
{
    std::vector<float> result;

    return result;
}

std::vector<float> Song::GetNoteTimestamps(const ChartDifficulty& diff)
{
    if (!mCharts.contains(diff))
        return std::vector<float>();

    std::vector<float> result;

    auto notes = mCharts[diff]->mNotes;
    for (auto& note : notes)
    {
        result.push_back(note->mTimestampSeconds);
    }

    return result;
}

std::vector<NoteCue> Song::GetNoteCues(const ChartDifficulty& diff)
{
    if (!mCharts.contains(diff))
        return std::vector<NoteCue>();

    std::vector<NoteCue> result;

    return result;
}
