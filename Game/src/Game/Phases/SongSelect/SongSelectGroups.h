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

	virtual bool IsLeaf() const = 0;
	virtual bool IsOpen() const = 0;

	virtual void OnOpen() = 0;
	virtual void OnFocus() = 0;
	virtual void OnUnfocus() = 0;

	virtual void Show() = 0;
	virtual void Hide() = 0;


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
	virtual inline bool IsLeaf() const override { return true; }
	virtual inline bool IsOpen() const override { return false; }

	void ConstructRenderable();

	virtual void OnOpen() override;
	virtual void OnFocus() override;
	virtual void OnUnfocus() override;

	virtual void Show() override;
	virtual void Hide() override;

private:
	Song* mSong;
	Renderable mCDRenderable;
};


struct SongSelectGroup : public SongSelectNode
{
	SongSelectGroup(const std::string& name, const unsigned childrenPerRow = 3);

	virtual inline bool IsLeaf() const override { return false; }
	virtual inline void Select() override { mIsOpen = !mIsOpen; }
	virtual inline bool IsOpen() const override { return mIsOpen; }

	virtual void OnOpen() override;
	virtual void OnFocus() override;
	virtual void OnUnfocus() override;

	virtual void Show() override;
	virtual void Hide() override;

	uint32_t GetChildrenPerRow() const { return mChildrenPerRow; }
	uint32_t GetDisplayedNodesCount() const;
	SongSelectNode* GetNodeByIdx(const uint32_t mSelectedIdx);

public:
	FontRenderer mLabelText;
	std::vector<std::shared_ptr<SongSelectNode>> mChildren;

private:
	bool mIsOpen = false;
	uint32_t mChildrenPerRow;
};