#include "Chart.h"
#include "Math/MathUtils.h"
#include "Utils/GameUtils.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <numeric>
#include <sstream>

Chart::Chart() : mStepArtist(""), mDifficultyCategory(ChartDifficulty::Beginner), mDifficultyVal(0) {}

Chart::Chart(const std::string& stepArtist, const std::string& difficulty, const unsigned difficultyVal)
    : mStepArtist(stepArtist), mDifficultyCategory(GameUtils::StrToChartDifficulty(difficulty)),
      mDifficultyVal(difficultyVal)
{
}

Chart::~Chart() {}

void Chart::ProcessNotes(std::istringstream& file)
{
    float totalMeasures = 0.0f;
    float lineCount = 0.0f;
    std::string line;
    std::vector<Note*> currNotes;
    std::array<HoldNote*, 4> pressingHolds = {nullptr, nullptr, nullptr, nullptr};
    std::array<float, 4> pressingHoldsMeasures = {0.0f, 0.0f, 0.0f, 0.0f};

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

                if (it->IsHoldNote())
                {
                    HoldNote* hold = dynamic_cast<HoldNote*>(it);
                    hold->SetPos(hold->mMeasurePos * 4.0f / lineCount + pressingHoldsMeasures[hold->mDir]);
                    hold->SetEnd(hold->mEnd * 4.0f / lineCount + totalMeasures);
                }
                else
                    it->SetPos(it->mMeasurePos * 4.0f / lineCount + totalMeasures);

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

void Chart::ProcessTimestamps(const MeasureMap<f32>& bpms, const MeasureMap<f32>& stops, const f32 offset)
{
    struct ChartEvent
    {
        enum class Type
        {
            BPM,
            STOP,
            NOTE
        };

        ChartEvent(Note* n) : note(n), type(Type::NOTE) {}
        ChartEvent(Type t, f32 val) : value(val), type(t) {}

        f32 value; // bpm for BPM, seconds for STOP
        Note* note = nullptr;

        Type type;
    };

    std::multimap<f32, ChartEvent> events;
    for (auto& n : mNotes)
    {
        events.emplace(n->mMeasurePos, ChartEvent(n));
    }
    for (auto& [measure, bpm] : bpms)
    {
        events.emplace(measure, ChartEvent(ChartEvent::Type::BPM, bpm));
    }
    for (auto& [measure, time] : stops)
    {
        events.emplace(measure, ChartEvent(ChartEvent::Type::STOP, time));
    }

    float currTimestamp = -offset;
    float lastMeasure = 0.0f;
    float currentBPM = bpms.begin()->second;

    for (const auto& [measure, e] : events)
    {
        float deltaM = measure - lastMeasure;
        currTimestamp += deltaM * (60.0f / currentBPM);
        lastMeasure = measure;

        switch (e.type)
        {
        case ChartEvent::Type::BPM:
            currentBPM = e.value;
            break;

        case ChartEvent::Type::STOP:
            currTimestamp += e.value;
            break;

        case ChartEvent::Type::NOTE:
            e.note->mTimestampSeconds = currTimestamp;
            break;
        }
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
    if (note->mType == Note::Type::MINE)
        return 'M';
    else if (note->mType == Note::Type::FAKE)
        return 'F';
    else if (note->mType == Note::Type::LIFT)
        return 'L';
    else if (note->mType == Note::Type::ROLL)
        return '4';
    else if (note->mType == Note::Type::HOLD)
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

                // @TODO: Fix this issue/reformat how this is done
                if (rowIdx >= 0)
                    measureStr[rowIdx][currHold.second] = '3';
            }

            for (auto currNote : noteList)
            {
                int rowIdx = static_cast<int>((currNote->mMeasurePos - currMinPos) * multipleNum);
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
        checkIfPrint(it->mMeasurePos);

        if (it->IsHoldNote())
            holdInfo.push_back({dynamic_cast<HoldNote*>(it)->mEnd, it->mDir});

        timingsList.push_back(getTimingFromPos(it->mMeasurePos));
        noteList.push_back(it);
    }

    while (!noteList.empty())
        checkIfPrint(noteList[0]->mMeasurePos + 4.0f);

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
        if (currNote->mType != Note::Type::MINE)
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
        if (prevNote && prevNote->mMeasurePos == currNote->mMeasurePos)
        {
            if (prevNote->mType == Note::Type::MINE || currNote->mType == Note::Type::MINE)
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
        if (currNote->mType == Note::Type::HOLD)
            holdNotes.insert(dynamic_cast<HoldNote*>(currNote));
    }

    return holdNotes;
}

std::multiset<RollNote*, NoteCompare> Chart::GetAllRollNotes()
{
    std::multiset<RollNote*, NoteCompare> rollNotes;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (currNote->mType == Note::Type::ROLL)
            rollNotes.insert(dynamic_cast<RollNote*>(currNote));
    }

    return rollNotes;
}

std::multiset<MineNote*, NoteCompare> Chart::GetAllMineNotes()
{
    std::multiset<MineNote*, NoteCompare> mineNotes;
    for (auto it = mNotes.begin(); it != mNotes.end(); ++it)
    {
        Note* currNote = *it;
        if (currNote->mType == Note::Type::MINE)
            mineNotes.insert(dynamic_cast<MineNote*>(currNote));
    }

    return mineNotes;
}
