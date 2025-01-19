#pragma once
#include "DevMenuItem.h"

struct DevMenuNode : public DevMenuItem
{
public:
    DevMenuNode(const std::string& name) : DevMenuItem(name) {}

    virtual void InitializeRenderables(int32_t yStartPos);
    void ShowRenderables();
    void HideRenderables();

    void AddItem(const std::shared_ptr<DevMenuItem>& item);
    void AddRenderable(const std::shared_ptr<Renderable>& renderable);

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
    std::vector<std::shared_ptr<Renderable>> mItemRenderables;
    std::vector<std::shared_ptr<Renderable>> mCustomRenderables;
    int32_t mSelectedIdx = 0;
};

struct DevMenuMainMenu : public DevMenuNode
{
	DevMenuMainMenu();
};

struct DevMenuInputOutputCheck : public DevMenuNode
{
    struct InputCheck : public DevMenuNode
    {
        InputCheck(DevMenuNode* parentNode);
    };

    struct FootPanelCheck : public DevMenuNode
    {
        FootPanelCheck(DevMenuNode* parentNode);
    };

    struct ICCardCheck : public DevMenuNode
    {
        ICCardCheck(DevMenuNode* parentNode);
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
    // .sm/.scc to (.smd + .scd)
    struct UpdateSongVersions : public DevMenuNode
    {
        UpdateSongVersions(DevMenuNode* parentNode);

        virtual void OnSelected() override;
        void UpdateSongs() const;

    private:
        void UpdateSong(const std::string& path) const;

        bool IsSongUpToDate(const std::string& path) const;
    };

    DevMenuGameSettings(DevMenuNode* parentNode);
};

struct DevMenuCoinSettings : public DevMenuNode
{
    DevMenuCoinSettings(DevMenuNode* parentNode);
};