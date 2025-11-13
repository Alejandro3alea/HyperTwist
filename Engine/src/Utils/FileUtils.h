#pragma once
#include <Json/json.hpp>
#include <string>

using json = nlohmann::json;

namespace FileUtils
{
std::string JoinPath(const std::string& lhs, const std::string& rhs);

std::optional<std::string> FindFirstFileOf(const std::filesystem::path& root, const std::string& ext);

void JsonToFile(const json& val, const std::string& path);
json FileToJson(const std::string& path);
} // namespace FileUtils