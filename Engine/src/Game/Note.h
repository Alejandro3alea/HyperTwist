#pragma once
#include "Graphics/Renderable.h"

typedef unsigned NoteID;

#define LeftDirection 0b00
#define DownDirection 0b01
#define UpDirection 0b10
#define RightDirection 0b11

typedef char Direction;

struct MineNote;

struct Note
{
	Note(const Direction dir, const float pos);

	virtual void SetPos(const float pos);

	bool operator<(const Note& rhs) const  { return mPos < rhs.mPos; }
	bool operator==(const Note& rhs) const { return mPos == rhs.mPos; }
	bool operator>(const Note& rhs) const  { return mPos >  rhs.mPos; }

	Direction mDir;
	float mPos; // In measures
};

struct HoldNote : public Note
{
	HoldNote(const Direction dir, const float pos, const float end = 0.0f);

	virtual void SetPos(const float pos);
	void SetEnd(const float pos);

	float mEnd;
};

struct MineNote : public Note
{
	MineNote(const Direction dir, const float pos) : Note(dir, pos) { }
};

struct FakeNote : public Note
{
	FakeNote(const Direction dir, const float pos) : Note(dir, pos) {}
};

struct LiftNote : public Note
{
	LiftNote(const Direction dir, const float pos) : Note(dir, pos) {}
};

struct RollNote : public HoldNote
{
	RollNote(const Direction dir, const float pos, const float end = 0.0f) : HoldNote(dir, pos, end) {}
};