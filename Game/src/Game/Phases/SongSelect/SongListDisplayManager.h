#pragma once
#include "SongSelectGroups.h"
#include "Composition/Events/Event.h"

CREATE_MULTICAST_EVENT(OnSongFocus, Song*);
CREATE_MULTICAST_EVENT(OnSongUnfocus, Song*);
CREATE_MULTICAST_EVENT(OnSongSelect, Song*);

class SongListDisplayManager
{
	struct SongListDisplayRow
	{
		explicit SongListDisplayRow(const size_t size) : mMaxSize(size) {}

		const size_t mMaxSize;
		std::vector<SongSelectNode*> mNodes;
	};

public:
	SongListDisplayManager() = default;

	void Reconstruct(SongSelectGroup* mainGroup);

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void Select();

	void Show();
	void Hide();
	void ResetIndices();

	void FocusNode();
	void UnfocusNode();

	void UpdateDisplay(bool force = false);

public:
	OnSongFocus mOnSongFocus;
	OnSongUnfocus mOnSongUnfocus;
	OnSongSelect mOnSongSelect;

private:
	void Construct(const SongSelectGroup* mainGroup);

	void OpenGroup(SongSelectGroup* group);

	void UpdateGroup(SongSelectGroup* group, const glm::vec3& newPos, const glm::vec3& newScale, bool isSelected, bool force);
	void UpdateSongNode(SongSelectSongNode* node, const glm::vec3& newPos, const glm::vec3& newScale, bool isSelected, bool force);
	void UpdateRow(int32_t idx, float yPos, bool force);
	void DisableRow(int32_t idx);

	void ResetVisibility();

	void SetNodeIndicesFrom(const SongSelectNode* node);

private:
	std::vector<SongListDisplayRow> mRows;
	uint32_t mDisplayedRows = 3;
	int32_t mMiddleRow = 0;
	int32_t mSelectedNode = 0;

	SongSelectGroup* mMainGroup = nullptr;
	SongSelectGroup* mOpenGroup = nullptr;
};