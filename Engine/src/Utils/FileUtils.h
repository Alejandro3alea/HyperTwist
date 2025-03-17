#pragma once
#include <json.hpp>
#include <string>

using json = nlohmann::json;

namespace FileUtils
{
    void JsonToFile(const json& val, const std::string& path);
    json FileToJson(const std::string& path);
}