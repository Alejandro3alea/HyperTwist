#pragma once
#include "SongSelectGroups.h"

// Main song select groups. It groups songs by a filter.
struct SongSelectFilter : public SongSelectGroup
{
	SongSelectFilter(const std::string& name) : SongSelectGroup(name, 1) {}

	virtual void GenerateChildren() = 0;
	virtual void GenerateRenderables() {}

	virtual void OnOpen() = 0;
};


struct SongSelectSortByName : public SongSelectFilter
{
	SongSelectSortByName() : SongSelectFilter("Sort by Name") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByLevel : public SongSelectFilter
{
	SongSelectSortByLevel() : SongSelectFilter("Sort by Level") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByArtist : public SongSelectFilter
{
	SongSelectSortByArtist() : SongSelectFilter("Sort by Artist") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByVersion : public SongSelectFilter
{
	SongSelectSortByVersion() : SongSelectFilter("Sort by Version") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }

private:
	std::map<std::string, std::vector<std::string>> LoadPlatformData();
};

struct SongSelectSortByBPM : public SongSelectFilter
{
	SongSelectSortByBPM() : SongSelectFilter("Sort by BPM") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByGenre : public SongSelectFilter
{
	SongSelectSortByGenre() : SongSelectFilter("Sort by Genre") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }

};