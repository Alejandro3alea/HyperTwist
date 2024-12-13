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

protected:
	std::string mName;
	bool mIsHovered = false;
	std::shared_ptr<Renderable> mRenderable;
};


struct SongSelectSongNode : public SongSelectNode
{
	SongSelectSongNode(Resource<Song>* song) : SongSelectNode(song->get()->mTitle), mSong(song) { ConstructRenderable(); }

	void ConstructRenderable();

private:
	Resource<Song>* mSong;
	std::shared_ptr<Renderable> mCDRenderable;
};


struct SongSelectGroup : public SongSelectNode
{
	SongSelectGroup(const std::string& name, const unsigned childrenPerRow = 3) : SongSelectNode(name), mChildrenPerRow(childrenPerRow) {}

	virtual void GenerateChildren() {}

protected:
	std::vector<std::shared_ptr<SongSelectNode>> mChildren;

private:
	unsigned mChildrenPerRow;
};


// Main song select groups. It groups songs by a filter.
struct SongSelectFilter : public SongSelectGroup
{
	SongSelectFilter(const std::string& name) : SongSelectGroup(name, 1) {}

	virtual void OnOpen() = 0;
};


struct SongSelectSortByName : public SongSelectFilter
{
	SongSelectSortByName();

	virtual void GenerateChildren() override;

	void OnOpen() {}

	std::vector<Resource<Song>*> mSongs;
};