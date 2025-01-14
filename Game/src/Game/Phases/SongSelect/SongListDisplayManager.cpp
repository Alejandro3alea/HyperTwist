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
				{
					result.push_back(static_cast<float>(i) / (nodeCount - 1.0f));
				}
			}

			return result;
		};

	int32_t midPoint = mDisplayedRows / 2 + 1;
	const float yInc = 900 / midPoint;
	float yPos = -900;
	const int32_t startRow = mMiddleRow - midPoint;
	const int32_t endRow = mMiddleRow + midPoint;
	const std::array<glm::vec3, 3> positions = {
		glm::vec3(440, 350, 10),
		glm::vec3(440, 0, 10),
		glm::vec3(440, -350, 10)
	};
	for (int32_t i = startRow; i < endRow; i++)
	{
		const uint32_t idx = (i >= 0) ? i : i + mRows.size();
		auto& rowNodes = mRows[idx].mNodes;
		const size_t rowNodeCount = rowNodes.size();
		//std::vector<float> xVals = getNodeDistributions(rowNodeCount);
		for (uint32_t j = 0; j < rowNodeCount; j++)
		{
			//float xPos = 400.0f * xVals[j] + 400.0f;
			auto& nodeT = rowNodes[j]->mRenderable.transform;
			nodeT.pos = positions[idx%3];
			nodeT.scale = glm::vec3(400.0f, 100.0f, 1.0f);
		}
		yPos += yInc;
	}

	glm::vec4& SelectedCol = mRows[mMiddleRow].mNodes[mSelectedNode]->mRenderable.mColor;
	SelectedCol = Math::Lerp(SelectedCol, glm::vec4(1.0f, 1.0f, 0.20f, 1.0f), 0.1f);
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