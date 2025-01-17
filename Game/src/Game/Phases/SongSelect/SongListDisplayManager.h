#pragma once
#include "SongSelectGroups.h"

class SongListDisplayManager
{
	struct SongListDisplayRow
	{
		SongListDisplayRow(const size_t size) : mMaxSize(size) {}

		const size_t mMaxSize;
		std::vector<SongSelectNode*> mNodes;
	};

public:
	void Reconstruct(SongSelectGroup* mainGroup);

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void Select();

	void FocusNode();
	void UnfocusNode();

	void UpdateDisplay();

private:
	void Construct(SongSelectGroup* mainGroup);

	void OpenGroup(SongSelectGroup* group);

	void UpdateGroup(SongSelectGroup* group, const glm::vec3& newPos, const glm::vec3& newScale, const bool isSelected);
	void UpdateSongNode(SongSelectSongNode* node, const glm::vec3& newPos, const glm::vec3& newScale, const bool isSelected);
	void UpdateRow(const int32_t idx, const float yPos);
	void DisableRow(const int32_t idx);

	void SetNodeIndicesFrom(SongSelectNode* node);

private:
	std::vector<SongListDisplayRow> mRows;
	uint32_t mDisplayedRows = 3;
	int32_t mMiddleRow = 0;
	int32_t mSelectedNode = 0;

	SongSelectGroup* mMainGroup;
	SongSelectGroup* mOpenGroup = nullptr;
};