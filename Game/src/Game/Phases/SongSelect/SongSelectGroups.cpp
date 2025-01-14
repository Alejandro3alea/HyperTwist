#include "SongSelectGroups.h"

#include <fstream>

void SongSelectSongNode::Select()
{
}

void SongSelectSongNode::ConstructRenderable()
{
	mRenderable.SetTexture("data/engine/texture/SongSelect/SongBase.png");
	mCDRenderable.SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
	mRenderable.transform.scale = glm::vec3(200.0f, 200.0f, 1.0f);
}

void SongSelectSongNode::OnOpen()
{

}

void SongSelectSongNode::OnFocus()
{
}

void SongSelectSongNode::OnUnfocus()
{
}
	
SongSelectGroup::SongSelectGroup(const std::string& name, const unsigned childrenPerRow) : SongSelectNode(name), mChildrenPerRow(childrenPerRow)
{
	mRenderable.SetTexture("data/engine/texture/SongSelect/SongBase.png");
	mRenderable.mbIsVisible = true;
	mRenderable.mColor = glm::vec4(1,1,1,1);
	std::cout << "Created " << name << std::endl;
}

void SongSelectGroup::OnOpen()
{
}

void SongSelectGroup::OnFocus()
{
	
}

void SongSelectGroup::OnUnfocus()
{
}

uint32_t SongSelectGroup::GetDisplayedNodesCount() const
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