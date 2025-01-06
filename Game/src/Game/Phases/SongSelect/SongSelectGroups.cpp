#include "SongSelectGroups.h"

void SongSelectSongNode::Select()
{
}

void SongSelectSongNode::ConstructRenderable()
{
	mRenderable = std::make_shared<Renderable>();
	mCDRenderable = std::make_shared<Renderable>();

	mRenderable->SetTexture("data/engine/texture/SongSelect/SongBase.png");
	mCDRenderable->SetTexture("data/engine/texture/SongSelect/RectangleHorizontalFade.png");
}

void SongSelectSortByName::GenerateChildren(const std::vector<Resource<Song>*>& songs)
{
	for (uint8_t c = 'A'; c <= 'Z'; c++)
	{
		mChildren.push_back(std::make_shared<SongSelectGroup>(std::to_string(c)));
	}
}

void SongSelectSortByBPM::GenerateChildren(const std::vector<Resource<Song>*>& songs)
{
	for (uint32_t bpm = 100; bpm <= 500; bpm += 20)
	{
		mChildren.push_back(std::make_shared<SongSelectGroup>(std::to_string(bpm)));
	}
}

void SongSelectSortByVersion::GenerateChildren(const std::vector<Resource<Song>*>& songs)
{
	std::map<std::string, std::vector<std::string>> versions = LoadPlatformData();
}

std::map<std::string, std::vector<std::string>> SongSelectSortByVersion::LoadPlatformData()
{
	std::map<std::string, std::vector<std::string>> list;
	std::ifstream file("data/local/Platform")


	return list;
}

void SongSelectSortByLevel::GenerateChildren(const std::vector<Resource<Song>*>& songs)
{
}


void SongSelectTestFilter::GenerateChildren(const std::vector<Resource<Song>*>& songs)
{
	std::shared_ptr<SongSelectGroup> newGroup = std::make_shared<SongSelectGroup>("Hello There");
	for (auto& song : songs)
	{
		newGroup->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));

	}
	mChildren.push_back(newGroup);
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

SongSelectNode* SongSelectGroup::GetNodeByIdx(const uint32_t mSelectedIdx)
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