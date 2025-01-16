#include "SongListDisplayManager.h"
#include "Math/Easing.h"

void SongListDisplayManager::Reconstruct(SongSelectGroup* mainGroup)
{
	mMainGroup = mainGroup;
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
	SongSelectNode* selectedNode = mRows[mMiddleRow].mNodes[mSelectedNode];
	selectedNode->OnOpen();
	selectedNode->mRenderable.mColor = glm::vec4(1.0f);

	if (!selectedNode->IsLeaf())
		OpenGroup(dynamic_cast<SongSelectGroup*>(selectedNode));

}

void SongListDisplayManager::FocusNode()
{
	mRows[mMiddleRow].mNodes[mSelectedNode]->OnFocus();
}

void SongListDisplayManager::UnfocusNode()
{
	mRows[mMiddleRow].mNodes[mSelectedNode]->OnUnfocus();
}


void SongListDisplayManager::UpdateGroup(SongSelectGroup* group, const glm::vec3& newPos, const glm::vec3& newScale, const bool isSelected)
{
	//float xPos = 400.0f * xVals[j] + 400.0f;
	group->Show();

	auto& baseT = group->mRenderable.transform;
	auto& labelT = group->mLabelText.transform;
	baseT.pos = Math::Lerp(baseT.pos, newPos, 0.2f);
	baseT.scale = Math::Lerp(baseT.scale, newScale, 0.2f);
	labelT.pos = Math::Lerp(labelT.pos, newPos, 0.2f);
	labelT.pos.z = 2.0f;

	const glm::vec4 updateCol = isSelected ? glm::vec4(1.0f, 1.0f, 0.20f, 1.0f) : glm::vec4(1.0f);
	glm::vec4& selectedCol = group->mRenderable.mColor;
	selectedCol = Math::Lerp(selectedCol, updateCol, 0.2f);
}

void SongListDisplayManager::UpdateSongNode(SongSelectSongNode* node, const glm::vec3& newPos, const glm::vec3& newScale, const bool isSelected)
{
	//float xPos = 400.0f * xVals[j] + 400.0f;
	node->Show();

	auto& baseT = node->mRenderable.transform;
	auto& labelT = node->mRenderable.transform;
	baseT.pos = Math::Lerp(baseT.pos, newPos, 0.2f);
	baseT.scale = Math::Lerp(baseT.scale, newScale, 0.2f);
	labelT.pos = Math::Lerp(labelT.pos, newPos, 0.2f);

	const glm::vec4 updateCol = isSelected ? glm::vec4(1.0f, 1.0f, 0.20f, 1.0f) : glm::vec4(1.0f);
	glm::vec4& selectedCol = node->mRenderable.mColor;
	selectedCol = Math::Lerp(selectedCol, updateCol, 0.2f);
}

void SongListDisplayManager::UpdateRow(const int32_t idx, const float yPos)
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

	const int32_t mappedIdx = (idx < 0) ? idx + mRows.size() : (idx >= mRows.size()) ? idx - mRows.size() : idx;
	auto& rowNodes = mRows[mappedIdx].mNodes;
	const size_t rowNodeCount = rowNodes.size();
	const std::vector<float> nodeDists = getNodeDistributions(rowNodeCount);

	const float distFromMidpoint = std::abs(mMiddleRow - idx);
	glm::vec3 newScale = glm::vec3(450.0f, 120.0f, 1.0f) - glm::vec3(50.0f, 20.0f, 0.0f) * distFromMidpoint;
	newScale /= rowNodeCount;


	//std::vector<float> xVals = getNodeDistributions(rowNodeCount);
	for (uint32_t i = 0; i < rowNodeCount; i++)
	{
		const bool isSelected = (distFromMidpoint == 0.0f && i == mSelectedNode);
		const glm::vec3 newPos(350.0f + nodeDists[i] * 200.0f, yPos, 1.0f);

		if (rowNodes[i]->IsLeaf())
			UpdateSongNode(dynamic_cast<SongSelectSongNode*>(rowNodes[i]), newPos, newScale, isSelected);
		else // group
			UpdateGroup(dynamic_cast<SongSelectGroup*>(rowNodes[i]), newPos, newScale, isSelected);

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

void SongListDisplayManager::SetNodeIndicesFrom(SongSelectNode* node)
{
	const size_t rowCount = mRows.size();
	for (uint32_t i = 0; i < rowCount; i++)
	{
		const size_t rowSize = mRows[i].mNodes.size();
		for (uint32_t j = 0; j < rowSize; j++)
		{
			SongSelectNode* currNode = mRows[i].mNodes[j];
			if (currNode == node)
			{
				mMiddleRow = i;
				mSelectedNode = j;
				return;
			}
		}
	}
}

void SongListDisplayManager::UpdateDisplay()
{
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

void SongListDisplayManager::Construct(SongSelectGroup* mainGroup)
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
			Construct(dynamic_cast<SongSelectGroup*>(group.get()));
		}
		else if (currRow.mNodes.size() == mainGroup->GetChildrenPerRow())
		{
			mRows.push_back(currRow);
			currRow.mNodes.clear();
		}
	}

	if (!currRow.mNodes.empty())
		mRows.push_back(currRow);
}

void SongListDisplayManager::OpenGroup(SongSelectGroup* group)
{
	if (mOpenGroup)
		mOpenGroup->Close();

	group->Open();
	Reconstruct(mMainGroup);

	SetNodeIndicesFrom(group);
	mOpenGroup = group;
}
