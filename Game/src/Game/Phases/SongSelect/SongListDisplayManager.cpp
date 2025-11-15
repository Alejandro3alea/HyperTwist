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
        mMiddleRow = (mMiddleRow != 0) ? mMiddleRow - 1 : mRows.size() - 1;
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
        mSelectedNode = 0;
        MoveDown();
        return;
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
    else
    {
        auto* songNode = dynamic_cast<SongSelectSongNode*>(selectedNode);
        mOnSongSelect.Broadcast(songNode->GetSong());
    }
}

void SongListDisplayManager::Show() { UpdateDisplay(true); }

void SongListDisplayManager::Hide()
{
    for (auto& row : mRows)
        for (auto it : row.mNodes)
            it->Hide();
}

void SongListDisplayManager::ResetIndices()
{
    mMiddleRow = 0;
    mSelectedNode = 0;
}

void SongListDisplayManager::FocusNode()
{
    SongSelectNode* node = mRows[mMiddleRow].mNodes[mSelectedNode];
    node->OnFocus();

    if (node->IsLeaf())
    {
        auto* songNode = dynamic_cast<SongSelectSongNode*>(node);
        mOnSongFocus.Broadcast(songNode->GetSong());
    }
}

void SongListDisplayManager::UnfocusNode()
{
    SongSelectNode* node = mRows[mMiddleRow].mNodes[mSelectedNode];
    node->OnUnfocus();
    if (node->IsLeaf())
    {
        auto* songNode = dynamic_cast<SongSelectSongNode*>(node);
        mOnSongUnfocus.Broadcast(songNode->GetSong());
    }
}

void SongListDisplayManager::UpdateGroup(SongSelectGroup* group, const glm::vec3& newPos, const glm::vec3& newScale,
                                         const bool isSelected, const bool force)
{
    group->Show();

    auto& baseT = group->mRenderable.transform;
    auto& labelT = group->mLabelText.transform;
    baseT.pos = force ? newPos : Math::Lerp(baseT.pos, newPos, 0.2f);
    baseT.scale = force ? newScale : Math::Lerp(baseT.scale, newScale, 0.2f);
    labelT.pos = force ? newPos : Math::Lerp(labelT.pos, newPos, 0.2f);
    labelT.pos.z = 2.0f;

    const glm::vec4 updateCol = isSelected ? glm::vec4(1.0f, 1.0f, 0.20f, 1.0f) : glm::vec4(1.0f);
    glm::vec4& selectedCol = group->mRenderable.mColor;
    selectedCol = Math::Lerp(selectedCol, updateCol, 0.2f);
}

void SongListDisplayManager::UpdateSongNode(SongSelectSongNode* node, const glm::vec3& newPos,
                                            const glm::vec3& newScale, const bool isSelected, const bool force)
{
    node->Show();

    auto& baseT = node->mRenderable.transform;
    auto& cdT = node->mCDRenderable.transform;
    auto& labelT = node->mLabel.transform;

    const glm::vec3 targetCDPos = newPos + glm::vec3(0.f, newScale.y / 5.f, 0.1f);
    const glm::vec3 targetLabelPos = newPos + glm::vec3(0.f, -newScale.y, 0.2f);
    const glm::vec3 targetBaseScale = glm::vec3(newScale.x, newScale.y * 1.2f, 1.f);
    const glm::vec3 targetCDScale = glm::vec3(newScale.x) / 1.25f;

    baseT.pos = force ? newPos : Math::Lerp(baseT.pos, newPos, 0.2f);
    baseT.scale = force ? targetBaseScale : Math::Lerp(baseT.scale, targetBaseScale, 0.2f);
    cdT.pos = force ? targetCDPos : Math::Lerp(cdT.pos, targetCDPos, 0.2f);
    cdT.scale = force ? targetCDScale : Math::Lerp(cdT.scale, targetCDScale, 0.2f);
    labelT.pos = force ? targetLabelPos : Math::Lerp(labelT.pos, targetLabelPos, 0.2f);

    const glm::vec4 updateCol = isSelected ? glm::vec4(0.7f, 0.7f, 0.f, 1.0f) : glm::vec4(0.f, 0.f, 0.f, 1.0f);
    glm::vec4& selectedCol = node->mRenderable.mColor;
    selectedCol = Math::Lerp(selectedCol, updateCol, 0.2f);
}

void SongListDisplayManager::UpdateRow(const i32 idx, const float yPos, const bool force)
{
    const auto getNodeDistributions = [](const i32 nodeCount) -> std::vector<float>
    {
        std::vector<float> result;
        switch (nodeCount)
        {
        case 0:
            return {};
        case 1:
            return std::vector<float>({0.5f});
        default:
            for (u32 i = 0; i < nodeCount; i++)
                result.push_back(static_cast<float>(i) / (nodeCount - 1.0f));
        }

        return result;
    };

    // @TODO: Check with one song, buggy with one song
    if (std::abs(idx) > static_cast<i32>(mRows.size()))
        return;

    const i32 mappedIdx = (idx < 0) ? idx + mRows.size() : (idx >= mRows.size()) ? idx - mRows.size() : idx;
    auto& rowNodes = mRows[mappedIdx].mNodes;
    const size_t rowNodeCount = mRows[mappedIdx].mMaxSize;
    const std::vector<float> nodeDists = getNodeDistributions(rowNodeCount);

    const float distFromMidpoint = std::abs(mMiddleRow - idx);
    const float rowNodesDivisionA = std::pow(rowNodeCount, 1.1f);
    const float rowNodesDivision = std::pow(rowNodeCount, 1.5f);
    glm::vec3 newScale = glm::vec3(450.0f / rowNodesDivisionA, 120.0f, 1.0f) -
                         glm::vec3(50.0f, 20.0f, 0.0f) * distFromMidpoint / rowNodesDivision;

    const float startX = distFromMidpoint * 175.0f;
    const float sizeX = 650.0f - distFromMidpoint * 100.0f;

    for (u32 i = 0; i < rowNodes.size(); i++)
    {
        const bool isSelected = (distFromMidpoint == 0.0f && i == mSelectedNode);
        const glm::vec3 newPos(startX + nodeDists[i] * sizeX, yPos, 1.0f);

        if (rowNodes[i]->IsLeaf())
            UpdateSongNode(dynamic_cast<SongSelectSongNode*>(rowNodes[i]), newPos, newScale, isSelected, force);
        else // group
            UpdateGroup(dynamic_cast<SongSelectGroup*>(rowNodes[i]), newPos, newScale, isSelected, force);
    }
}

void SongListDisplayManager::DisableRow(const int32_t idx)
{
    if (std::abs(idx) > mRows.size())
        return;

    const uint32_t mappedIdx = (idx < 0) ? idx + mRows.size() : (idx >= mRows.size()) ? idx - mRows.size() : idx;
    auto& rowNodes = mRows[mappedIdx].mNodes;
    for (SongSelectNode* rowNode : rowNodes)
        rowNode->Hide();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void SongListDisplayManager::ResetVisibility()
{
    const size_t rowCount = mRows.size();
    for (uint32_t i = 0; i < rowCount; i++)
    {
        const size_t rowSize = mRows[i].mNodes.size();
        for (uint32_t j = 0; j < rowSize; j++)
        {
            mRows[i].mNodes[j]->Hide();
        }
    }
}

void SongListDisplayManager::SetNodeIndicesFrom(const SongSelectNode* node)
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

void SongListDisplayManager::UpdateDisplay(const bool force)
{
    int32_t midPoint = static_cast<int32_t>(mDisplayedRows) / 2 + 1;
    const float yInc = 700.0f / static_cast<float>(midPoint);
    float yPos = 700.0f;
    const int32_t startRow = mMiddleRow - midPoint;
    const int32_t endRow = mMiddleRow + midPoint;
    for (int32_t i = startRow; i <= endRow; i++)
    {
        UpdateRow(i, yPos, force);
        yPos -= yInc;
    }
    DisableRow(startRow - 1);
    DisableRow(endRow + 1);
}

void SongListDisplayManager::Construct(const SongSelectGroup* mainGroup)
{
    if (mainGroup == nullptr)
        return;

    SongListDisplayRow currRow(mainGroup->GetChildrenPerRow());
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
    ResetVisibility();

    if (mOpenGroup)
        mOpenGroup->Close();

    group->Open();
    Reconstruct(mMainGroup);

    SetNodeIndicesFrom(group);
    mOpenGroup = group;
}
