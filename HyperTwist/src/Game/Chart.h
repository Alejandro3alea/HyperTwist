#pragma once
#include "Note.h"

#include <string>
#include <vector>

enum ChartDifficulty
{
	Beginner,
	Easy,
	Medium,
	Hard,
	Challenge,
	Edit,
	None
};

struct Chart
{
	Chart();
	Chart(const std::string& stepArtist, const std::string& difficulty, const unsigned difficultyVal);

	static ChartDifficulty ProcessDifficulty(const std::string& difficulty);
	
	void ProcessNotes(std::istringstream& file);

public:
	ChartDifficulty mDifficulty;
	unsigned mDifficultyVal = 1;
	std::string mStepArtist;

	std::vector<Note*> mNotes;
};