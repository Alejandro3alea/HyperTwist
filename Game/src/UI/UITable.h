#pragma once
#include "UIElementData.h"

#include <map>
#include <string>
#include <filesystem>

#define SET_TABLE_VALUES(tableName, name) \
name.transform.pos   = tableName[#name].pos;   \
name.transform.scale = tableName[#name].scale; \
name.mColor = tableName[#name].color;

struct UITable
{
  	explicit UITable(std::string path);

    void Update();

    void ReloadData();
    [[nodiscard]] bool IsDataUpToDate() const;

    [[nodiscard]] UIElementData GetElementData(const std::string& element) const;

	UIElementData& operator[](const std::string& element) { return mDataMap[element]; }

private:
  	std::map<std::string, UIElementData> mDataMap;

    const std::string mDataPath;
    std::filesystem::file_time_type mLastModificationDate = {};
};
