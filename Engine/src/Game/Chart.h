#pragma once
#include "Note.h"

#include <string>
#include <set>

struct NoteCompare 
{
	bool operator()(const Note* a, const Note* b) const 
	{
		return *a < *b;
	}
};

enum ChartDifficulty
{
	Beginner,
	Easy,
	Medium,
	Hard,
	Challenge,
	Special,
	None
};

struct Chart
{
	Chart();
	Chart(const std::string& stepArtist, const std::string& difficulty, const unsigned difficultyVal);

	~Chart();
	
	void ProcessNotes(std::istringstream& file);
	void SaveNotes(std::ofstream& file);

	std::multiset<Note*, NoteCompare> GetAllNormalNotes();
	std::vector<std::vector<Note*>> GetAllJumps();
	std::multiset<HoldNote*, NoteCompare> GetAllHoldNotes();
	std::multiset<RollNote*, NoteCompare> GetAllRollNotes();
	std::multiset<MineNote*, NoteCompare> GetAllMineNotes();

public:
	ChartDifficulty mDifficultyCategory;
	int mDifficultyVal = 1;
	std::string mStepArtist;

	std::multiset<Note*, NoteCompare> mNotes;

	NoteRenderer* mNoteRenderer = nullptr;
	HoldNoteBodyRenderer* mHoldRenderer = nullptr;
	MineRenderer* mMineRenderer = nullptr;
};