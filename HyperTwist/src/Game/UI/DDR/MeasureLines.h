#pragma once
#include "Renderable.h"
#include "Game/Song.h"

struct MeasureLines
{
	MeasureLines() = default;

	void OnSongChange(Song* InSong);

private:
	std::unique_ptr<LineList> mLineList;
	std::unique_ptr<Renderable> mNotesBackground;
	std::unique_ptr<Renderable> mBeatsBackground;
};