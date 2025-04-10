#pragma once
#include <json.hpp>
#include <string>

using json = nlohmann::json;

namespace FileUtils
{
    std::string JoinPath(const std::string& lhs, const std::string& rhs);

    void JsonToFile(const json& val, const std::string& path);
    json FileToJson(const std::string& path);
}