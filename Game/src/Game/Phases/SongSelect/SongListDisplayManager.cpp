#include "SongListDisplayManager.h"
#include "Math/Easing.h"

void SongListDisplayManager::Reconstruct(const SongSelectGroup* mainGroup)
{
	mMiddleRow = 0;
	mRows.clear();
	Construct(mainGroup);
}

void SongListDisplayManager::MoveUp()
{
	UnfocusNode();
	mMiddleRow = (mMiddleRow != 0) ? mMiddleRow - 1 : mRows.size() - 1;
	mSelectedNode = std::min(mSelectedNode, static_cast<int32_t>(mRows[mMiddleRow].mNodes.size() - 1));
	FocusNode();
}

void SongListDisplayManager::MoveDown()
{
	UnfocusNode();
	mMiddleRow = (mMiddleRow != mRows.size() - 1) ? mMiddleRow + 1 : 0;
	mSelectedNode = std::min(mSelectedNode, static_cast<int32_t>(mRows[mMiddleRow].mNodes.size() - 1));
	FocusNode();
}

void SongListDisplayManager::MoveLeft()
{
	UnfocusNode();
	if (mSelectedNode == 0)
	{
		MoveUp();
		mSelectedNode = mRows[mMiddleRow].mNodes.size() - 1;
	}
	else
		mSelectedNode--;

	FocusNode();
}

void SongListDisplayManager::MoveRight()
{
	UnfocusNode();
	if (mSelectedNode == mRows[mMiddleRow].mNodes.size() - 1)
	{
		MoveDown();
		mSelectedNode = 0;
	}
	else
		mSelectedNode++;

	FocusNode();
}

void SongListDisplayManager::Select()
{
	mRows[mMiddleRow].mNodes[mSelectedNode]->OnOpen();
}

void SongListDisplayManager::FocusNode()
{
	mRows[mMiddleRow].mNodes[mSelectedNode]->OnFocus();
}

void SongListDisplayManager::UnfocusNode()
{
	mRows[mMiddleRow].mNodes[mSelectedNode]->OnUnfocus();
}


void SongListDisplayManager::UpdateGroup(SongSelectGroup* group, const float yPos, const bool isSelected)
{
	//float xPos = 400.0f * xVals[j] + 400.0f;
	group->Show();
	const glm::vec3 newPos(440.0f, yPos, 1.0f);

	auto& baseT = group->mRenderable.transform;
	auto& labelT = group->mLabelText.transform;
	baseT.pos = Math::Lerp(baseT.pos, newPos, 0.2f);
	baseT.scale = glm::vec3(400.0f, 100.0f, 1.0f);
	labelT.pos = Math::Lerp(labelT.pos, newPos, 0.2f);

	const glm::vec4 updateCol = isSelected ? glm::vec4(1.0f, 1.0f, 0.20f, 1.0f) : glm::vec4(1.0f);
	glm::vec4& selectedCol = group->mRenderable.mColor;
	selectedCol = Math::Lerp(selectedCol, updateCol, 0.2f);
}

void SongListDisplayManager::UpdateSongNode(SongSelectSongNode* node, const float yPos, const bool isSelected)
{
	//float xPos = 400.0f * xVals[j] + 400.0f;
	node->Show();
	const glm::vec3 newPos(440.0f, yPos, 1.0f);

	auto& baseT = node->mRenderable.transform;
	auto& labelT = node->mRenderable.transform;
	baseT.pos = Math::Lerp(baseT.pos, newPos, 0.2f);
	baseT.scale = glm::vec3(400.0f, 100.0f, 1.0f);
	labelT.pos = Math::Lerp(labelT.pos, newPos, 0.2f);
	labelT.pos.z = 2.0f;

	const glm::vec4 updateCol = isSelected ? glm::vec4(1.0f, 1.0f, 0.20f, 1.0f) : glm::vec4(1.0f);
	glm::vec4& selectedCol = node->mRenderable.mColor;
	selectedCol = Math::Lerp(selectedCol, updateCol, 0.2f);
}

void SongListDisplayManager::UpdateRow(const int32_t idx, const float yPos)
{
	const uint32_t mappedIdx = (idx < 0) ? idx + mRows.size() : (idx >= mRows.size()) ? idx - mRows.size() : idx;
	auto& rowNodes = mRows[mappedIdx].mNodes;
	const size_t rowNodeCount = rowNodes.size();
	
	//std::vector<float> xVals = getNodeDistributions(rowNodeCount);
	for (uint32_t i = 0; i < rowNodeCount; i++)
	{
		const bool isSelected = (mappedIdx == mMiddleRow && i == mSelectedNode);
		if (rowNodes[i]->IsLeaf())
			UpdateSongNode(dynamic_cast<SongSelectSongNode*>(rowNodes[i]), yPos, isSelected);
		else // group
			UpdateGroup(dynamic_cast<SongSelectGroup*>(rowNodes[i]), yPos, isSelected);

	}
}

void SongListDisplayManager::DisableRow(const int32_t idx)
{
	const uint32_t mappedIdx = (idx < 0) ? idx + mRows.size() : (idx >= mRows.size()) ? idx - mRows.size() : idx;
	auto& rowNodes = mRows[mappedIdx].mNodes;
	for (SongSelectNode* rowNode : rowNodes)
	{
		rowNode->Hide();
	}
}

void SongListDisplayManager::UpdateDisplay()
{
	const auto getNodeDistributions = [](const uint32_t nodeCount) -> std::vector<float>
		{
			std::vector<float> result;
			switch (nodeCount)
			{
			case 0:
				return std::vector<float>();
			case 1:
				return std::vector<float>({ 0.5f });
			default:
				for (uint32_t i = 0; i < nodeCount; i++)
					result.push_back(static_cast<float>(i) / (nodeCount - 1.0f));
			}

			return result;
		};

	int32_t midPoint = mDisplayedRows / 2 + 1;
	const float yInc = 700 / midPoint;
	float yPos = 700;
	const int32_t startRow = mMiddleRow - midPoint;
	const int32_t endRow = mMiddleRow + midPoint;
	for (int32_t i = startRow; i <= endRow; i++)
	{
		UpdateRow(i, yPos);
		yPos -= yInc;
	}
	DisableRow(startRow - 1);
	DisableRow(endRow + 1);
}

void SongListDisplayManager::Construct(const SongSelectGroup* mainGroup)
{
	if (mainGroup == nullptr)
		return;

	SongListDisplayRow currRow;
	for (auto& group : mainGroup->mChildren)
	{
		currRow.mNodes.push_back(group.get());
		if (group->IsOpen())
		{
			mRows.push_back(currRow);
			currRow.mNodes.clear();
			Construct(mainGroup);
		}
		else if (currRow.mNodes.size() == mainGroup->GetChildrenPerRow())
		{
			mRows.push_back(currRow);
			currRow.mNodes.clear();
		}
	}
}