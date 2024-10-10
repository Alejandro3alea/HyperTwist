#pragma once
#include "DevMenuItem.h"

struct DevMenuNode;

struct DevMenuActionLeaf : public DevMenuItem
{
    DevMenuActionLeaf(const std::string& name, std::function<void()> action);

    void OnSelected() override
    {
        mAction();
    }

private:
    std::function<void()> mAction;
};

struct DevMenuStartGame : public DevMenuActionLeaf
{
    DevMenuStartGame();

    void StartGame();
};

struct DevMenuReturnToParent : public DevMenuActionLeaf
{
    DevMenuReturnToParent(DevMenuNode* parentNode);

    void ReturnToParent();

public:
    DevMenuNode* mParentNode;
};