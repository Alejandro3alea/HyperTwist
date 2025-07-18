#include "SongSelectGroups.h"

#include <fstream>

void SongSelectSongNode::Select()
{
}

void SongSelectSongNode::ConstructRenderable()
{
	mRenderable.SetTexture("engine/texture/SongSelect/SongBase.png");
	mRenderable.transform.scale = glm::vec3(200.0f, 200.0f, 1.0f);
	mRenderable.mbIsVisible = false;

	mCDRenderable.mbIsVisible = false;

	mLabel.transform.scale = glm::vec3(0.4f);
	mLabel.SetText(mSong->mSongInfo.mTitle);
	mLabel.mbIsVisible = false;
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
	mLabel.mbIsVisible = true;

	const SongInfo& info = mSong->mSongInfo;
	if (!info.mCDTitlePath.empty())
		mCDRenderable.mTexture = ResourceMgr->Load<Texture>(mSong->GetPath() + info.mCDTitlePath);
	else if(!info.mBannerPath.empty())
		mCDRenderable.mTexture = ResourceMgr->Load<Texture>(mSong->GetPath() + info.mBannerPath);
}

void SongSelectSongNode::Hide()
{
	mRenderable.mbIsVisible = false;
	mCDRenderable.mbIsVisible = false;
	mLabel.mbIsVisible = false;
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

SongSelectNode* SongSelectGroup::GetNodeByIdx(const uint32_t mSelectedIdx) const
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
