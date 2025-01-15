#pragma once
#include "SongSelectGroups.h"

class SongListDisplayManager
{
	struct SongListDisplayRow
	{
		std::vector<SongSelectNode*> mNodes;
	};

public:
	void Reconstruct(const SongSelectGroup* mainGroup);

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void Select();

	void FocusNode();
	void UnfocusNode();

	void UpdateDisplay();

private:
	void Construct(const SongSelectGroup* mainGroup);

	void UpdateRow(const int32_t idx, const float yPos);
	void DisableRow(const int32_t idx);

private:
	std::vector<SongListDisplayRow> mRows;
	uint32_t mDisplayedRows = 3;
	int32_t mMiddleRow = 0;
	int32_t mSelectedNode = 0;
};