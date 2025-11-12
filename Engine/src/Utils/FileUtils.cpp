#include "FileUtils.h"
#include "Misc/Requires.h"

#include <fstream>

namespace FileUtils
{
std::string JoinPath(const std::string& lhs, const std::string& rhs)
{
    if (rhs.starts_with(lhs))
        return rhs;

    std::string fixedRhs = rhs;
    if (!fixedRhs.empty() && fixedRhs.front() == '/')
        fixedRhs.erase(0, 1);

    std::filesystem::path fullPath =
        static_cast<std::filesystem::path>(lhs) / static_cast<std::filesystem::path>(fixedRhs);

    return fullPath.generic_string();
}

void JsonToFile(const nlohmann::json& val, const std::string& path)
{
    std::ofstream file(path);
    Requires(file.is_open() && file.good(), "Failed to open path for saving json");

    if (file.is_open())
    {
        // For indentation
        file << val.dump(4);
        file.close();
    }
}

json FileToJson(const std::string& path)
{
    std::ifstream file(path);
    Requires(file.is_open() && file.good(), "Failed to open path for loading json");

    // Read contents
    std::string jsonString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Convert file to json
    json val = json::parse(jsonString);
    file.close();

    return val;
}
} // namespace FileUtils
