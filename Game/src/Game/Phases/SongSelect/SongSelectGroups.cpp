#include "SongSelectGroups.h"

#include <fstream>

void SongSelectSongNode::Select()
{
}

void SongSelectSongNode::ConstructRenderable()
{
	mRenderable.SetTexture("data/engine/texture/SongSelect/SongBase.png");
	mCDRenderable.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
}

void SongSelectSongNode::OnOpen()
{

}
	
void SongSelectGroup::OnOpen()
{
}

uint32_t SongSelectGroup::GetDisplayedNodesCount()
{
	uint32_t result = 0;

	for (auto& child : mChildren)
	{
		if (!child->IsLeaf())
		{
			result += dynamic_cast<SongSelectGroup*>(child.get())->GetDisplayedNodesCount();
		}

		result++;
	}

	return result;
}

SongSelectNode* SongSelectGroup::GetNodeByIdx(uint32_t mSelectedIdx)
{
	for (uint32_t i = 0; i < mChildren.size(); i++)
	{
		auto child = mChildren[i];

		if (i == mSelectedIdx)
			return child.get();

		if (!child->IsLeaf())
		{
			SongSelectNode* selectedNode = dynamic_cast<SongSelectGroup*>(child.get())->GetNodeByIdx(mSelectedIdx - i - 1);
			if (selectedNode != nullptr)
				return selectedNode;
		}
	}

	return nullptr;
}