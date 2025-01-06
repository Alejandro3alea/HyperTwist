#pragma once
#include "Resources/ResourceMgr.h"
#include "Graphics/Renderable.h"
#include "Game/Song.h"

#include <vector>

struct Renderable;


struct SongSelectNode
{
	SongSelectNode(const std::string& name) : mName(name) {}

	virtual ~SongSelectNode() {}

	virtual void Select() = 0;
	virtual inline bool IsLeaf() = 0;

protected:
	std::string mName;
	bool mIsHovered = false;
	std::shared_ptr<Renderable> mRenderable;
};


struct SongSelectSongNode : public SongSelectNode
{
	SongSelectSongNode(Resource<Song>* song) : SongSelectNode(song->get()->mTitle), mSong(song) { ConstructRenderable(); }

	virtual void Select() override;
	virtual inline bool IsLeaf() override { return true; }

	void ConstructRenderable();

private:
	Resource<Song>* mSong;
	std::shared_ptr<Renderable> mCDRenderable;
};


struct SongSelectGroup : public SongSelectNode
{
	SongSelectGroup(const std::string& name, const unsigned childrenPerRow = 3) : SongSelectNode(name), mChildrenPerRow(childrenPerRow) {}

	virtual inline bool IsLeaf() override { return false; }
	virtual void Select() override { mIsOpen = !mIsOpen; }
	bool IsOpen() { return mIsOpen; }

	uint32_t GetDisplayedNodesCount();
	SongSelectNode* GetNodeByIdx(const uint32_t mSelectedIdx);

public:
	std::vector<std::shared_ptr<SongSelectNode>> mChildren;

private:
	bool mIsOpen = false;
	unsigned mChildrenPerRow;
};


// Main song select groups. It groups songs by a filter.
struct SongSelectFilter : public SongSelectGroup
{
	SongSelectFilter(const std::string& name, const std::vector<Resource<Song>*>& songs) : SongSelectGroup(name, 1) { GenerateChildren(songs); }

	virtual void GenerateChildren(const std::vector<Resource<Song>*>& songs) {}
	virtual void GenerateRenderables() {}

	virtual void OnOpen() = 0;
};


struct SongSelectSortByName : public SongSelectFilter
{
	SongSelectSortByName(const std::vector<Resource<Song>*>& songs) : SongSelectFilter("Sort by Song Name", songs) {}

	virtual void GenerateChildren(const std::vector<Resource<Song>*>& songs) override;

	void OnOpen() {}
};

struct SongSelectSortByLevel : public SongSelectFilter
{
	SongSelectSortByLevel(const std::vector<Resource<Song>*>& songs) : SongSelectFilter("Sort by Song Name", songs) {}

	virtual void GenerateChildren(const std::vector<Resource<Song>*>& songs) override;

	void OnOpen() {}
};

struct SongSelectSortByVersion : public SongSelectFilter
{
	SongSelectSortByVersion(const std::vector<Resource<Song>*>& songs) : SongSelectFilter("Sort by Song Name", songs) {}

	virtual void GenerateChildren(const std::vector<Resource<Song>*>& songs) override;

	void OnOpen() {}

private:
	std::map<std::string, std::vector<std::string>> versions = LoadPlatformData()
};

struct SongSelectSortByBPM : public SongSelectFilter
{
	SongSelectSortByBPM(const std::vector<Resource<Song>*>& songs) : SongSelectFilter("Sort by Song Name", songs) {}

	virtual void GenerateChildren(const std::vector<Resource<Song>*>& songs) override;

	void OnOpen() {}
};

struct SongSelectTestFilter : public SongSelectFilter
{
	SongSelectTestFilter(const std::vector<Resource<Song>*>& songs) : SongSelectFilter("Test filter", songs) {}

	virtual void GenerateChildren(const std::vector<Resource<Song>*>& songs) override;

	void OnOpen() {}

};