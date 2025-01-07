#include "SongSelectFilters.h"
#include "SongSelectPhase.h"

#include <fstream>

void SongSelectSortByName::GenerateChildren()
{
	auto songs = SongSelectPhase::GetSongsByName();
	for (uint8_t c = 'A'; c <= 'Z'; c++)
	{
		std::shared_ptr<SongSelectGroup> currChild = std::make_shared<SongSelectGroup>(std::to_string(c));
		for (Song* song : songs[c])
		{
			currChild->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));
		}
		mChildren.push_back(currChild);
	}
}

void SongSelectSortByLevel::GenerateChildren()
{
	auto songs = SongSelectPhase::GetSongsByLevel();
	for (uint8_t lvl = 1; lvl <= 20; lvl++)
	{
		std::shared_ptr<SongSelectGroup> currChild = std::make_shared<SongSelectGroup>(std::to_string(lvl));
		for (Song* song : songs[lvl])
		{
			currChild->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));
		}
		mChildren.push_back(currChild);
	}
}

void SongSelectSortByArtist::GenerateChildren()
{
	auto songs = SongSelectPhase::GetSongsByArtist();
	for (uint8_t c = 'A'; c <= 'Z'; c++)
	{
		std::shared_ptr<SongSelectGroup> currChild = std::make_shared<SongSelectGroup>(std::to_string(c));
		for (Song* song : songs[c])
		{
			currChild->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));
		}
		mChildren.push_back(currChild);
	}
}

void SongSelectSortByVersion::GenerateChildren()
{
	auto songs = SongSelectPhase::GetSongsByPlatform();
	std::map<std::string, std::vector<std::string>> data = LoadPlatformData();
	for (auto& [platform, versions] : data)
	{
		std::shared_ptr<SongSelectGroup> platformChild = std::make_shared<SongSelectGroup>(platform);

		// Create all songs group
		std::shared_ptr<SongSelectGroup> allSongsChild = std::make_shared<SongSelectGroup>("All Songs");
		const std::vector<Song*>& platformSongs = songs[platform];
		for (Song* song : platformSongs)
		{
			allSongsChild->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));
		}
		platformChild->mChildren.push_back(allSongsChild);

		// Create group per version
		for (const std::string& version : versions)
		{
			std::shared_ptr<SongSelectGroup> versionChild = std::make_shared<SongSelectGroup>(version);
			const std::vector<Song*>& versionSongs = SongSelectPhase::GetSongsByVersion(platformSongs, version);
			for (Song* song : versionSongs)
			{
				versionChild->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));
			}
			platformChild->mChildren.push_back(versionChild);
		}
		mChildren.push_back(platformChild);
	}
}

std::map<std::string, std::vector<std::string>> SongSelectSortByVersion::LoadPlatformData()
{
	std::map<std::string, std::vector<std::string>> list;

	std::ifstream file("data/local/PlatformCategories.data");
	if (!file || !file.is_open())
	{
		PrintWarning("\"PlatformCategories.data\" not found.");
		return list;
	}

	std::string currPlatform;
	std::vector<std::string> currVersions;
	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty())
		{
			currPlatform.clear();
			currVersions.clear();
		}
		else if (currPlatform.empty())
		{
			currPlatform = line;
		}
		else
		{
			currVersions.push_back(line.substr(2));
		}
	}

	return list;
}

void SongSelectSortByBPM::GenerateChildren()
{
	constexpr uint32_t bmpIncrement = 20;
	for (uint32_t bpm = 100; bpm <= 500; bpm += bmpIncrement)
	{
		const std::string groupStr = std::to_string(bpm) + "-" + std::to_string(bpm + bmpIncrement);
		std::shared_ptr<SongSelectGroup> bpmChild = std::make_shared<SongSelectGroup>(groupStr);
		auto songs = SongSelectPhase::GetSongsFromBPMRange(bpm, bpm + bmpIncrement);
		for (Song* song : songs)
		{
			bpmChild->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));
		}
		mChildren.push_back(bpmChild);
	}
}

void SongSelectSortByGenre::GenerateChildren()
{
	/*std::shared_ptr<SongSelectGroup> newGroup = std::make_shared<SongSelectGroup>("Hello There");
	for (auto& song : songs)
	{
		newGroup->mChildren.push_back(std::make_shared<SongSelectSongNode>(song));

	}
	mChildren.push_back(newGroup);*/
}
