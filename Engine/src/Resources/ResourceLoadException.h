#pragma once
#include <string>

class ResourceLoadException : public std::exception
{
public:
	explicit ResourceLoadException(const std::string& pathStr, const std::string& reasonStr = "") : path(pathStr), reason(reasonStr) {}

public:
	const std::string path;
	const std::string reason;
};