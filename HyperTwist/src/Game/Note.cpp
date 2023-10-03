#include "Note.h"
#include "Editor.h"

Note::Note(const Direction dir, const float pos) : mDir(dir), mPos(pos)
{
	mAtlas.mTexture = ResourceMgr->Load<Texture>("data/noteskins/USWCelSM5/_Down Tap Note 16x8.png");
	mAtlas.SetTextureScale(64, 64);
	mAtlas.SetTextureOffset(0, 448);
}

void Note::SetPos(const float pos)
{
	// @TODO: Note styles
	const std::pair<float, unsigned> coords[] = {
		{ 0.0f,			448 },
		{ 1.0f / 2.0f,	384 },
		{ 1.0f / 3.0f,	320 },
		{ 2.0f / 3.0f,	320 },
		{ 1.0f / 4.0f,	256 },
		{ 3.0f / 4.0f,	256 },
		{ 1.0f / 6.0f,	192 },
		{ 5.0f / 6.0f,	192 },
		{ 1.0f / 8.0f,	128 },
		{ 3.0f / 8.0f,	128 },
		{ 5.0f / 8.0f,	128 },
		{ 7.0f / 8.0f,	128 },
		{ 1.0f / 12.0f,	64 },
		{ 5.0f / 12.0f,	64 },
		{ 7.0f / 12.0f,	64 },
		{ 11.0f / 12.0f,64 },
		{ 1.0f / 5.0f,	0 }		
	};

	const float xPositions[4] = { -3.0f, -1.0f, 1.0f, 3.0f };
	const float rotations[4] = { -90.0f, 0.0f, 180.0f, 90.0f };
	mPos = pos;
	mAtlas.transform.pos = glm::vec3(xPositions[mDir], pos * Editor->mZoom, 1.0f);
	mAtlas.transform.rotation = rotations[mDir];

	const float decimalPart = pos - static_cast<int>(pos);
	unsigned currbeatUVOffset;
	for (unsigned i = 0; i < 8; i++)
	{
		currbeatUVOffset = coords[i].second;
		if (coords[i].first == decimalPart)
			break;
	}
	mAtlas.SetTextureOffset(0, currbeatUVOffset);
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
