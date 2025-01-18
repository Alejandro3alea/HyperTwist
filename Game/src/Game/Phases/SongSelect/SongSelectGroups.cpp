#include "SongSelectGroups.h"

#include <fstream>

void SongSelectSongNode::Select()
{
}

void SongSelectSongNode::ConstructRenderable()
{
	mRenderable.SetTexture("data/engine/texture/SongSelect/SongBase.png");
	mRenderable.mbIsVisible = false;
	mRenderable.transform.scale = glm::vec3(200.0f, 200.0f, 1.0f);

	mCDRenderable.mbIsVisible = false;
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

void SongSelectSongNode::Show()
{
	mRenderable.mbIsVisible = true;
	mCDRenderable.mbIsVisible = true;

	if (!mSong->mCDTitlePath.empty())
		mCDRenderable.mTexture = ResourceMgr->Load<Texture>(mSong->GetPath() + mSong->mCDTitlePath);
	else if(!mSong->mBannerPath.empty())
		mCDRenderable.mTexture = ResourceMgr->Load<Texture>(mSong->GetPath() + mSong->mBannerPath);
}

void SongSelectSongNode::Hide()
{
	mRenderable.mbIsVisible = false;
	mCDRenderable.mbIsVisible = false;
}
	
SongSelectGroup::SongSelectGroup(const std::string& name, const unsigned childrenPerRow) : SongSelectNode(name), mChildrenPerRow(childrenPerRow)
{
	mLabelText.SetText(name);
	mLabelText.mbIsVisible = false;
	mLabelText.mColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
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

void SongSelectGroup::Show()
{
	mRenderable.mbIsVisible = true;
	mLabelText.mbIsVisible = true;
}

void SongSelectGroup::Hide()
{
	mRenderable.mbIsVisible = false;
	mLabelText.mbIsVisible = false;
}

uint32_t SongSelectGroup::GetDisplayedNodesCount() const
{
	uint32_t result = 0;

	for (auto& child : mChildren)
	{
		if (!child->IsLeaf())
			result += dynamic_cast<SongSelectGroup*>(child.get())->GetDisplayedNodesCount();

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