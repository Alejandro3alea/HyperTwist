#include "Chart.h"

#include <sstream>
#include <fstream>
#include <array>

Chart::Chart() : mStepArtist(""), mDifficulty(ChartDifficulty::Beginner), mDifficultyVal(0) {}

Chart::Chart(const std::string& stepArtist, const std::string& difficulty, const unsigned difficultyVal) :
    mStepArtist(stepArtist), mDifficulty(ProcessDifficulty(difficulty)), mDifficultyVal(difficultyVal)
{
}

ChartDifficulty Chart::ProcessDifficulty(const std::string& difficulty)
{
	if (difficulty == "Beginner")
		return ChartDifficulty::Beginner;
	if (difficulty == "Easy")
		return ChartDifficulty::Easy;
	if (difficulty == "Medium")
		return ChartDifficulty::Medium;
	if (difficulty == "Hard")
		return ChartDifficulty::Hard;
	if (difficulty == "Challenge")
		return ChartDifficulty::Challenge;

	return ChartDifficulty::Edit;
}

void Chart::ProcessNotes(std::istringstream& file)
{
    float totalMeasures = 0.0f;
    float lineCount = 0.0f;
    std::string line;
    std::vector<Note*> currNotes;
    std::array<HoldNote*, 4> pressingHolds = { nullptr, nullptr, nullptr, nullptr };

    while (std::getline(file, line))
    {
        bool eoChart = false;
        if (line.find(';') != std::string::npos)
            eoChart = true;
        if (line.find(',') != std::string::npos || eoChart)
        {
            for (Note* it : currNotes)
            {
                it->SetPos(it->mPos * 4.0f / lineCount + totalMeasures);

                if (HoldNote* hold = dynamic_cast<HoldNote*>(it))
                    hold->SetEnd(hold->mEnd * 4.0f / lineCount + totalMeasures);

                mNotes.push_back(it);
            }
            currNotes.clear();

            totalMeasures += 4.0f;
            lineCount = 0.0f;

            if (eoChart)
                return;

            continue;
        }

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
                break;
            case '4':
                pressingHolds[noteDir] = new RollNote(noteDir, lineCount);
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
            case 'F':
                currNotes.push_back(new FakeNote(noteDir, lineCount));
            case 'L':
                currNotes.push_back(new LiftNote(noteDir, lineCount));
            }

            noteDir++;
        }

        lineCount++;
    }
}
