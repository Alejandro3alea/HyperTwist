#include "Note.h"

Note::Note(const Direction dir, const float pos) : mType(Type::NOTE), mDir(dir) { SetPos(pos); }

void Note::SetPos(const float pos)
{
    // @TODO: Note styles
    const std::pair<float, unsigned> coords[] = {
        {0.0f, 448},         {1.0f / 2.0f, 384}, {1.0f / 3.0f, 320}, {2.0f / 3.0f, 320}, {1.0f / 4.0f, 256},
        {3.0f / 4.0f, 256},  {1.0f / 6.0f, 192}, {5.0f / 6.0f, 192}, {1.0f / 8.0f, 128}, {3.0f / 8.0f, 128},
        {5.0f / 8.0f, 128},  {7.0f / 8.0f, 128}, {1.0f / 12.0f, 64}, {5.0f / 12.0f, 64}, {7.0f / 12.0f, 64},
        {11.0f / 12.0f, 64}, {1.0f / 5.0f, 0}};

    mMeasurePos = pos;
}

HoldNote::HoldNote(const Direction dir, const float pos, const float end) : Note(dir, pos), mEnd(end)
{
    mType = Type::HOLD;
}

void HoldNote::SetPos(const float pos) { Note::SetPos(pos); }

void HoldNote::SetEnd(const float pos) { mEnd = pos; }
