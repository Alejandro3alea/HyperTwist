#include "UITable.h"
#include "UITableException.h"
#include "Utils/FileUtils.h"
#include "Misc/Serialization.h"

#include <json.hpp>
#include <utility>

using json = nlohmann::json;

UITable::UITable(std::string path) : mDataPath(FileUtils::JoinPath(DATA_PATH, path))
{
    ReloadData();
}

void UITable::Update()
{
    if (!IsDataUpToDate())
        ReloadData();
}

void UITable::ReloadData()
{
    mDataMap.clear();
    json data = FileUtils::FileToJson(mDataPath)["Elements"];
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        json& currVal = it.value();
        glm::vec2 scale;
        UIElementData currData;
        if (currVal.contains("pos"))
            currData.pos << currVal["pos"];

        if (currVal.contains("scale"))
        {
            scale << currVal["scale"];
            currData.scale = glm::vec3(scale, 1.0f);
        }

        if (currVal.contains("color"))
            currData.color << currVal["color"];

        std::string name;
        name << currVal["name"];
        mDataMap[name] = currData;
    }

    mLastModificationDate = std::filesystem::last_write_time(mDataPath);
}

bool UITable::IsDataUpToDate() const
{
    if (!std::filesystem::exists(mDataPath))
        return false;

    const auto time = std::filesystem::last_write_time(mDataPath);
    return time == mLastModificationDate;
}

UIElementData UITable::GetElementData(const std::string& element) const
{
    if (!mDataMap.contains(element))
        throw UITableException("[UI] - Element couldn't be found.");

    return mDataMap.at(element);
}