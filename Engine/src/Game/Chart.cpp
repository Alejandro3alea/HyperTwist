#include "Chart.h"
#include "Math/MathUtils.h"
#include "Utils/GameUtils.h"

#include <sstream>
#include <fstream>
#include <array>
#include <numeric>
#include <algorithm>

Chart::Chart() : mStepArtist(""), mDifficultyCategory(ChartDifficulty::Beginner), mDifficultyVal(0) 
{
}

Chart::Chart(const std::string& stepArtist, const std::string& difficulty, const unsigned difficultyVal) :
    mStepArtist(stepArtist), mDifficultyCategory(GameUtils::StrToChartDifficulty(difficulty)), mDifficultyVal(difficultyVal)
{
}

Chart::~Chart()
{
}

void Chart::ProcessNotes(std::istringstream& file)
{
    float totalMeasures = 0.0f;
    float lineCount = 0.0f;
    std::string line;
    std::vector<Note*> currNotes;
    std::array<HoldNote*, 4> pressingHolds = { nullptr, nullptr, nullptr, nullptr };
    std::array<float, 4> pressingHoldsMeasures = { 0.0f, 0.0f, 0.0f, 0.0f };

    while (std::getline(file, line))
    {
        bool eoChart = false;
        if (line.find(';') != std::string::npos)
            eoChart = true;
        if (line.find(',') != std::string::npos || eoChart)
        {
            for (Note* it : currNotes)
            {
                if (!it)
                    continue;

                if (HoldNote* hold = dynamic_cast<HoldNote*>(it))
                {
                    hold->SetPos(hold->mPos * 4.0f / lineCount + pressingHoldsMeasures[hold->mDir]);
                    hold->SetEnd(hold->mEnd * 4.0f / lineCount + totalMeasures);
                }
                else 
                    it->SetPos(it->mPos * 4.0f / lineCount + totalMeasures);

                mNotes.insert(it);
            }
            currNotes.clear();

            totalMeasures += 4.0f;
            lineCount = 0.0f;

            if (eoChart)
            {
                return;
            }

            continue;
        }

        // If not end of measure (eom),  processnotes
        Direction noteDir = LeftDirection;
        for (char currVal : line)
        {
            switch (currVal)
            {
            case '1':
                currNotes.push_back(new Note(noteDir, lineCount));
                break;
            case '2':
                pressingHolds[noteDir] = new HoldNote(noteDir, lineCount);
                pressingHoldsMeasures[noteDir] = totalMeasures;
                break;
            case '4':
                pressingHolds[noteDir] = new RollNote(noteDir, lineCount);
                pressingHoldsMeasures[noteDir] = totalMeasures;
                break;
            case '3':
                if (pressingHolds[noteDir] != nullptr)
                {
                    pressingHolds[noteDir]->mEnd = lineCount;
                    currNotes.push_back(pressingHolds[noteDir]);
                    pressingHolds[noteDir] = nullptr;
                }
                break;
            case 'M':
                currNotes.push_back(new MineNote(noteDir, lineCount));
                break;
            case 'F':
                currNotes.push_back(new FakeNote(noteDir, lineCount));
                break;
            case 'L':
                currNotes.push_back(new LiftNote(noteDir, lineCount));
                break;
            }

            noteDir++;
        }

        lineCount++;
    }
}

int GetMultipleOfBiggetsPrimeNumbers(const std::vector<int>& nums) 
{
    std::map<int, int> maxFactors;

    for (int num : nums) 
    {
        auto factors = Math::GetPrimeFactors(num);
        for (const auto& [factor, count] : factors) 
        {
            maxFactors[factor] = std::max(maxFactors[factor], count);
        }
    }

    int lcm = 1;
    for (const auto& [factor, count] : maxFactors)
        lcm *= static_cast<int>(pow(factor, count));
    return lcm;
}

char GetNoteTypeChar(Note* note)
{
    if (dynamic_cast<MineNote*>(note) != nullptr)
        return 'M';
    else if (dynamic_cast<FakeNote*>(note) != nullptr)
        return 'F';
    else if (dynamic_cast<LiftNote*>(note) != nullptr)
        return 'L';
    else if (dynamic_cast<RollNote*>(note) != nullptr)
        return '4';
    else if (dynamic_cast<HoldNote*>(note) != nullptr)
        return '2';
    else
        return '1';
}

void Chart::SaveNotes(std::ofstream& file)
{
    auto getTimingFromPos = [&](const float notePos) -> unsigned
    {
        const float MAX_NUM = 512.0f;
        for (float i = 1.0f; i <= MAX_NUM; i++)
        {
            const float multRes = notePos * i;
            const unsigned castRes = static_cast<unsigned>(multRes);
            if (multRes == static_cast<float>(castRes))
                return static_cast<unsigned>(i);
        }

        return 1;
    };

    bool isFirstMeasures = true;
    float currMinPos = 0.0f;
    float currMaxPos = 4.0f;
    std::vector<Note*> noteList;
    std::vector<int> timingsList;
    using HoldInfo = std::pair<float, Direction>;
    std::vector<HoldInfo> holdInfo;

    auto checkIfPrint = [&](const float notePos)
    {
        while (notePos >= currMaxPos)
        {
            // In first measure we don't print a comma
            if (isFirstMeasures)
                isFirstMeasures = false;
            else
                file << ',' << std::endl;

            std::vector<HoldInfo> holdEndsToAdd;
            for (unsigned i = 0; i < holdInfo.size();)
            {
                auto currHold = holdInfo[i];
                if (currHold.first < currMaxPos)
                {
                    timingsList.push_back(getTimingFromPos(currHold.first));
                    holdEndsToAdd.push_back(currHold);
                    holdInfo.erase(std::find(holdInfo.begin(), holdInfo.end(), currHold));
                }
                else
                    i++;
            }

            int multipleNum = GetMultipleOfBiggetsPrimeNumbers(timingsList);
            int rowCount = 4 * multipleNum;

            std::vector<std::string> measureStr;
            for (unsigned i = 0; i < rowCount; i++)
                measureStr.push_back("0000");

            for (auto currHold : holdEndsToAdd)
            {
                int rowIdx = static_cast<int>((currHold.first - currMinPos) * multipleNum);
                
                // TODO: Fix this issue/reformat how this is done
                if (rowIdx >= 0)
                    measureStr[rowIdx][currHold.second] = '3';
            }

            for (auto currNote : noteList)
            {
                int rowIdx = static_cast<int>((currNote->mPos - currMinPos) * multipleNum);
                measureStr[rowIdx][currNote->mDir] = GetNoteTypeChar(currNote);
            }
            for (unsigned i = 0; i < rowCount; i++)
                file << measureStr[i] << std::endl;

            measureStr.clear();
            holdEndsToAdd.clear();
            timingsList.clear();
            noteList.clear();

            currMinPos += 4.0f;
            currMaxPos += 4.0f;
        }
    };

    for (auto it : mNotes)
    {
        checkIfPrint(it->mPos);

        if (HoldNote* holdNote = dynamic_cast<HoldNote*>(it))
            holdInfo.push_back({ holdNote->mEnd, holdNote->mDir });

        timingsList.push_back(getTimingFromPos(it->mPos));
        noteList.push_back(it);
    }

    while (!noteList.empty())
        checkIfPrint(noteList[0]->mPos + 4.0f);

    while (!holdInfo.empty())
        checkIfPrint(holdInfo[0].first + 4.0f);

    file << ";" << std::endl;
}

std::multiset<Note*, NoteCompare> Chart::GetAllNormalNotes()
{
    std::multiset<Note*, NoteCompare> normalNotes;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (dynamic_cast<MineNote*>(currNote) == nullptr)
            normalNotes.insert(currNote);
    }

    return normalNotes;
}

std::vector<std::vector<Note*>> Chart::GetAllJumps()
{
    Note* prevNote = nullptr;
    std::vector<Note*> currJump;
    std::vector<std::vector<Note*>> jumpsList;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (prevNote && prevNote->mPos == currNote->mPos)
        {
            if (dynamic_cast<MineNote*>(prevNote) || dynamic_cast<MineNote*>(currNote))
                continue;

            if (currJump.empty())
                currJump.push_back(prevNote);

            currJump.push_back(currNote);
        }
        else 
        {
            if (currJump.size() >= 2)
                jumpsList.push_back(currJump);

            currJump.clear();
        }

        prevNote = currNote;
    }

    if (currJump.size() >= 2)
        jumpsList.push_back(currJump);

    return jumpsList;
}

std::multiset<HoldNote*, NoteCompare> Chart::GetAllHoldNotes()
{
    std::multiset<HoldNote*, NoteCompare> holdNotes;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (HoldNote* holdNote = dynamic_cast<HoldNote*>(currNote))
            holdNotes.insert(holdNote);
    }

    return holdNotes;
}

std::multiset<RollNote*, NoteCompare> Chart::GetAllRollNotes()
{
    std::multiset<RollNote*, NoteCompare> rollNotes;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (RollNote* rollNote = dynamic_cast<RollNote*>(currNote))
            rollNotes.insert(rollNote);
    }

    return rollNotes;
}

std::multiset<MineNote*, NoteCompare> Chart::GetAllMineNotes()
{
    std::multiset<MineNote*, NoteCompare> mineNotes;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (MineNote* mineNote = dynamic_cast<MineNote*>(currNote))
            mineNotes.insert(mineNote);
    }

    return mineNotes;
}
