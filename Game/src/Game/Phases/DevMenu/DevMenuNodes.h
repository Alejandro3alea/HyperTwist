#pragma once
#include "DevMenuItem.h"

struct DevMenuNode : public DevMenuItem
{
public:
    DevMenuNode(const std::string& name) : DevMenuItem(name) {}

    virtual void InitializeRenderables();
    void ShowRenderables();
    void HideRenderables();

    void AddItem(std::shared_ptr<DevMenuItem> item);

    void OnSelected() override;
    void Select();

    void ResetSelectedIdx();
    void IncrementSelectedIdx();
    void DecrementSelectedIdx();

private:
    void UpdateSelectedIdx();
    void UpdateRenderables();

private:
    std::vector<std::shared_ptr<DevMenuItem>> mItems;
    std::vector<std::shared_ptr<Renderable>> mRenderables;
    int32_t mSelectedIdx = 0;
};

struct DevMenuMainMenu : public DevMenuNode
{
	DevMenuMainMenu();
};

struct DevMenuInputOutputCheck : public DevMenuNode
{
    struct DevMenuInputCheck : public DevMenuNode
    {
        DevMenuInputCheck(DevMenuNode* parentNode);
    };

    struct DevMenuFootPanelCheck : public DevMenuNode
    {
        DevMenuFootPanelCheck(DevMenuNode* parentNode);
    };

    struct DevMenuICCardCheck : public DevMenuNode
    {
        DevMenuICCardCheck(DevMenuNode* parentNode);
    };

    DevMenuInputOutputCheck(DevMenuNode* parentNode);
};

struct DevMenuScreenCheck : public DevMenuNode
{
	DevMenuScreenCheck(DevMenuNode* parentNode);
};

struct DevMenuColorCheck : public DevMenuNode
{
    DevMenuColorCheck(DevMenuNode* parentNode);
};

struct DevMenuSoundSettings : public DevMenuNode
{
    DevMenuSoundSettings(DevMenuNode* parentNode);
};

struct DevMenuGameSettings : public DevMenuNode
{
    DevMenuGameSettings(DevMenuNode* parentNode);
};

struct DevMenuCoinSettings : public DevMenuNode
{
    DevMenuCoinSettings(DevMenuNode* parentNode);
};