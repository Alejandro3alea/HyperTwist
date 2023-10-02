#pragma once
#include "Renderable.h"

typedef unsigned NoteID;

#define LeftDirection 0b00
#define DownDirection 0b01
#define UpDirection 0b10
#define RightDirection 0b11

typedef char Direction;

struct Note
{
	virtual ~Note() {}

	Note(const Direction dir, const float pos);

	virtual void SetPos(const float pos);

	TextureAtlas mAtlas;
	Direction mDir;
	float mPos; // In measures
};

struct HoldNote : public Note
{
	virtual ~HoldNote() override {}

	HoldNote(const Direction dir, const float pos, const float end = 0.0f);

	virtual void SetPos(const float pos);
	void SetEnd(const float pos);

	float mEnd;
};

struct MineNote : public Note
{
	virtual ~MineNote() override {}

	MineNote(const Direction dir, const float pos) : Note(dir, pos) { mAtlas.SetTextureOffset(64, 128); }
};

struct FakeNote : public Note
{
	virtual ~FakeNote() override {}

	FakeNote(const Direction dir, const float pos) : Note(dir, pos) {}
};

struct LiftNote : public Note
{
	virtual ~LiftNote() override {}

	LiftNote(const Direction dir, const float pos) : Note(dir, pos) {}
};

struct RollNote : public HoldNote
{
	virtual ~RollNote() override {}

	RollNote(const Direction dir, const float pos, const float end = 0.0f) : HoldNote(dir, pos, end) {}
};