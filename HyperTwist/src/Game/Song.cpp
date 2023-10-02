#include "Song.h"
#include "Ensure.h"
#include "Audio/AudioMgr.h"
#include "Graphics/GfxMgr.h"
#include "Editor.h"

#include <sstream>
#include <fstream>
#include <algorithm>
#include <limits>

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
	Ensure(file.is_open() && file.good(), "File not read in " + path);

    std::string fileContents((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    RemoveContents(fileContents);
    std::istringstream cleanContents(fileContents);

    std::string extension = ResourceMgr->GetExtension(path);
    if (extension == "sm")
        ProcessSMSong(cleanContents);
    else if (extension == "ssc")
        ProcessSSCSong(cleanContents);

	file.close();
    
    GetResources();
}

void Song::Play()
{
    mSong->get()->Play();
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
        // Obtener el iterador al segundo elemento
        auto it = mBPMs.begin();
        std::advance(it, 1);

        // Iterar por los valores desde el segundo elemento
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

void Song::GetResources()
{
    // There should be ALWAYS a song path (No audios, no bitches)
    mSong = ResourceMgr->Load<Audio>(mPath + mSongPath);

    if (!mBannerPath.empty())
        mBanner     = ResourceMgr->Load<Texture>(mPath + mBannerPath);
    if (!mBackgroundPath.empty())
    {
        mBackground = ResourceMgr->Load<Texture>(mPath + mBackgroundPath);
        GfxMgr->SetBackground(mBackground);
    }
    if (!mCDTitlePath.empty())
        mCDTitle    = ResourceMgr->Load<Texture>(mPath + mCDTitlePath);
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

            bool isEoC = false;
            if (value.find(';') != std::string::npos)
            {
                isEoC = true;
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
                while (!isEoC)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEoC |= currLine.find(';') != std::string::npos;

                    if (isEoC)
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
            else if (key == "#NOTES")
            {
                mCharts.push_back(ProcessSMChart(file));
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

            bool isEoC = false;
            if (value.find(';') != std::string::npos)
            {
                isEoC = true;
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
                while (!isEoC)
                {
                    std::string currLine;
                    std::getline(file, currLine);
                    value += currLine;
                    isEoC |= currLine.find(';') != std::string::npos;

                    if (isEoC)
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
            else if (key == "#NOTEDATA")
            {
                mCharts.push_back(ProcessSSCChart(file));
            }
        }
    }
}

Chart* Song::ProcessSMChart(std::istringstream& file)
{
    auto charCheckLambda = [](unsigned char c) { return std::isspace(c) || c == ':'; };
    auto charEraseLambda = [charCheckLambda](std::string& str) { str.erase(std::remove_if(str.begin(), str.end(), charCheckLambda), str.end()); };
    auto commentEraseLambda = [](std::string& str) 
    {
        size_t commentPos = str.find("//");
    };
    std::string danceCategory;
    std::getline(file, danceCategory); charEraseLambda(danceCategory);
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
    auto charCheckLambda = [](unsigned char c) { return std::isspace(c) || c == ':'; };
    auto charEraseLambda = [charCheckLambda](std::string& str) { str.erase(std::remove_if(str.begin(), str.end(), charCheckLambda), str.end()); };
    auto commentEraseLambda = [](std::string& str)
    {
        size_t commentPos = str.find("//");
    }; 

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

            bool isEoC = false;
            if (value.find(';') != std::string::npos)
            {
                isEoC = true;
                value.pop_back();
            }
            /*

//--------------- dance-single -  ----------------
#NOTEDATA:;
#STEPSTYPE:dance-single;
#DESCRIPTION:;
#DIFFICULTY:Challenge;
#METER:9;
#RADARVALUES:0,0,0,0,0;
#NOTES:
0000
*/
            if (key == "#STEPSTYPE")
                stepType = value;
            else if (key == "#DESCRIPTION")
                currChart->mStepArtist = value;
            else if (key == "#DIFFICULTY")
                currChart->mDifficulty = Chart::ProcessDifficulty(value);
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
}
