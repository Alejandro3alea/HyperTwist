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

	void Select() override;
	bool IsLeaf() const override { return true; }
	bool IsOpen() const override { return false; }

	void ConstructRenderable();

	void OnOpen() override;
	void OnFocus() override;
	void OnUnfocus() override;

	void Show() override;
	void Hide() override;

	Song* GetSong() { return mSong; }

public:
	Renderable mCDRenderable;
	FontRenderer mLabel;

private:
	Song* mSong;
};


struct SongSelectGroup : public SongSelectNode
{
	explicit SongSelectGroup(const std::string& name, unsigned childrenPerRow = 3);

	bool IsLeaf() const override { return false; }
	void Select() override { mIsOpen = !mIsOpen; }
	bool IsOpen() const override { return mIsOpen; }

	void Open() { mIsOpen = true; OnOpen(); }
	void Close() { mIsOpen = false; OnClose(); }

	void OnClose() {}
	void OnOpen() override;
	void OnFocus() override;
	void OnUnfocus() override;

	void Show() override;
	void Hide() override;

	uint32_t GetChildrenPerRow() const { return mChildrenPerRow; }
	uint32_t GetDisplayedNodesCount() const;
	SongSelectNode* GetNodeByIdx(uint32_t mSelectedIdx) const;

public:
	FontRenderer mLabelText;
	std::vector<std::shared_ptr<SongSelectNode>> mChildren;

private:
	bool mIsOpen = false;
	uint32_t mChildrenPerRow;
};