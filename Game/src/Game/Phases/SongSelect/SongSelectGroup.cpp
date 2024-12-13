#include "SongSelectGroup.h"

void SongSelectSongNode::ConstructRenderable()
{
	mRenderable = std::make_shared<Renderable>();
	mCDRenderable = std::make_shared<Renderable>();

	mRenderable->SetTexture("data/engine/texture/SongSelect/SongBase.png");
	mCDRenderable->SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
}

SongSelectSortByName::SongSelectSortByName() : SongSelectFilter("Sort by name")
{
}

void SongSelectSortByName::GenerateChildren()
{
	for (uint8_t c = 'A'; c <= 'Z'; c++)
	{
		mChildren.push_back(std::make_shared<SongSelectGroup>(std::to_string(c)));
	}
}
