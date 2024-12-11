#pragma once
#include <vector>

struct SongSelectGroup
{
	SongSelectGroup(unsigned childrenPerRow = 3) : mChildrenPerRow(childrenPerRow) {}

private:
	unsigned mChildrenPerRow;
	std::vector<SongSelectGroup> mChildren;
};

struct SongSelectSong
{

};

// Main song select groups. It groups songs by a filter.
struct SongSelectFilter : public SongSelectGroup
{
	
};

