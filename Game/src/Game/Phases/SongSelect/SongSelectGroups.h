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
	virtual void OnOpen() = 0;

public:
	Renderable mRenderable;

protected:
	std::string mName;
	bool mIsHovered = false;
};


struct SongSelectSongNode : public SongSelectNode
{
	SongSelectSongNode(Song* song) : SongSelectNode(song->mTitle), mSong(song) { ConstructRenderable(); }
	
	virtual void Select() override;
	virtual inline bool IsLeaf() override { return true; }

	void ConstructRenderable();

	virtual void OnOpen() override;

private:
	Song* mSong;
	Renderable mCDRenderable;
};


struct SongSelectGroup : public SongSelectNode
{
	SongSelectGroup(const std::string& name, const unsigned childrenPerRow = 3) : SongSelectNode(name), mChildrenPerRow(childrenPerRow) {}

	virtual inline bool IsLeaf() override { return false; }
	virtual void Select() override { mIsOpen = !mIsOpen; }
	inline bool IsOpen() { return mIsOpen; }

	virtual void OnOpen() override;

	virtual uint32_t GetChildrenPerRow() { return mChildrenPerRow; }
	uint32_t GetDisplayedNodesCount();
	SongSelectNode* GetNodeByIdx(const uint32_t mSelectedIdx);

public:
	std::vector<std::shared_ptr<SongSelectNode>> mChildren;

private:
	bool mIsOpen = false;
	uint32_t mChildrenPerRow;
};