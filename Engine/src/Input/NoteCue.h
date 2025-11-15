#pragma once

struct Note;

struct NoteCue
{
    Note* mNote;
    f32 mTime;
};

struct NoteCueCompare
{
    bool operator()(const NoteCue& lhs, const NoteCue& rhs) { return lhs.mTime > rhs.mTime; }
};