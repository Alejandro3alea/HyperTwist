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
	mSelectedNode = std::min(mSelectedNode, static_cast<uint32_t>(mRows[mMiddleRow].mNodes.size() - 1));
	FocusNode();
}

void SongListDisplayManager::MoveDown()
{
	UnfocusNode();
	mMiddleRow = (mMiddleRow != mRows.size() - 1) ? mMiddleRow + 1 : 0;
	mSelectedNode = std::min(mSelectedNode, static_cast<uint32_t>(mRows[mMiddleRow].mNodes.size() - 1));
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



void SongListDisplayManager::UpdateDisplay()
{
	int32_t midPoint = mDisplayedRows / 2 + 1;
	const float yInc = 900 / midPoint;
	float yPos = -900;
	for (int32_t i = mMiddleRow - midPoint; i < mMiddleRow + midPoint; i++)
	{
		const uint32_t idx = (i >= 0) ? i : i + mRows.size();
		auto& rowNodes = mRows[mMiddleRow].mNodes;
		const size_t rowNodeCount = rowNodes.size();
		for (uint32_t j = 0; j < rowNodeCount; j++)
		{
			
		}
		yPos += yInc;
	}

	glm::vec4& SelectedCol = mRows[mMiddleRow].mNodes[mSelectedNode]->mRenderable.mColor;
	SelectedCol = Math::Lerp(SelectedCol, glm::vec4(1.0f, 1.0f, 0.20f, 1.0f), 0.05f);
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