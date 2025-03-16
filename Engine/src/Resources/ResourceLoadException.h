#pragma once
#include <string>

class ResourceLoadException : public std::exception
{
public:
	explicit ResourceLoadException(const std::string& pathStr, const std::string& reasonStr = "") : path(pathStr), reason(reasonStr) {}

	_NODISCARD char const* what() const override;
public:
	const std::string path;
	const std::string reason;
};

inline char const* ResourceLoadException::what() const
{
	return reason.c_str();
}
