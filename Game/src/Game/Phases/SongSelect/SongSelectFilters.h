#pragma once
#include "SongSelectGroups.h"

// Main song select groups. It groups songs by a filter.
struct SongSelectFilter : public SongSelectGroup
{
	SongSelectFilter(const std::string& name) : SongSelectGroup(name, 1) { SetupFilterTexture(name); }

	virtual void GenerateChildren() = 0;
	virtual void OnOpen() = 0;

	virtual void SetupFilterTexture(const std::string& name);
};


struct SongSelectSortByName : public SongSelectFilter
{
	SongSelectSortByName() : SongSelectFilter("SortByName") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByLevel : public SongSelectFilter
{
	SongSelectSortByLevel() : SongSelectFilter("SortByLevel") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByArtist : public SongSelectFilter
{
	SongSelectSortByArtist() : SongSelectFilter("SortByArtist") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByVersion : public SongSelectFilter
{
	SongSelectSortByVersion() : SongSelectFilter("SortByVersion") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }

private:
	std::map<std::string, std::vector<std::string>> LoadPlatformData();
};

struct SongSelectSortByBPM : public SongSelectFilter
{
	SongSelectSortByBPM() : SongSelectFilter("SortByBPM") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }
};

struct SongSelectSortByGenre : public SongSelectFilter
{
	SongSelectSortByGenre() : SongSelectFilter("SortByGenre") {}

	virtual void GenerateChildren() override;

	void OnOpen() { GenerateChildren(); }

};