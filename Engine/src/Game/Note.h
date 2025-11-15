#pragma once
#include "Graphics/Renderable.h"
#include "Misc/DataTypes.h"

typedef unsigned NoteID;

#define LeftDirection 0b00
#define DownDirection 0b01
#define UpDirection 0b10
#define RightDirection 0b11

typedef char Direction;

struct MineNote;

struct Note
{
    enum class Type : u8
    {
        NOTE,
        HOLD,
        MINE,
        FAKE,
        LIFT,
        ROLL
    };

    Note(const Direction dir, const float pos);

    virtual void SetPos(const float pos);

    bool operator<(const Note& rhs) const { return mMeasurePos < rhs.mMeasurePos; }
    bool operator==(const Note& rhs) const { return mMeasurePos == rhs.mMeasurePos; }
    bool operator>(const Note& rhs) const { return mMeasurePos > rhs.mMeasurePos; }

    bool IsHoldNote() { return mType == Type::HOLD || mType == Type::ROLL; }

    // Setting like this for padding purposes
    float mMeasurePos;
    float mTimestampSeconds;
    Type mType;
    Direction mDir;
    // + 2 extra pad bytes
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
    MineNote(const Direction dir, const float pos) : Note(dir, pos) { mType = Type::MINE; }
};

struct FakeNote : public Note
{
    FakeNote(const Direction dir, const float pos) : Note(dir, pos) { mType = Type::FAKE; }
};

struct LiftNote : public Note
{
    LiftNote(const Direction dir, const float pos) : Note(dir, pos) { mType = Type::LIFT; }
};

struct RollNote : public HoldNote
{
    RollNote(const Direction dir, const float pos, const float end = 0.0f) : HoldNote(dir, pos, end)
    {
        mType = Type::ROLL;
    }
};