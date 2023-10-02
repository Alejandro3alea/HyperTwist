#include "Note.h"

Note::Note(const Direction dir, const float pos) : mDir(dir), mPos(pos)
{
	mAtlas.mTexture = ResourceMgr->Load<Texture>("data/noteskins/metal/tex notes.png");
	mAtlas.SetTextureScale(64, 64);
	mAtlas.SetTextureOffset(0, 192);
}

void Note::SetPos(const float pos)
{
	const float xPositions[4] = { -3.0f, -1.0f, 1.0f, 3.0f };
	mPos = pos;
	mAtlas.transform.pos = glm::vec3(xPositions[mDir], pos * 2.0f, 1.0f);
}

HoldNote::HoldNote(const Direction dir, const float pos, const float end) : Note(dir, pos), mEnd(end)
{

}

void HoldNote::SetPos(const float pos)
{
	Note::SetPos(pos);
}

void HoldNote::SetEnd(const float pos)
{
}
