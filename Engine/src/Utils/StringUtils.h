#pragma once
#include <algorithm>
#include <string>

std::string ToLower(const std::string& str)
{
	std::string data(str);
	std::transform(data.begin(), data.end(), data.begin(), [](const uint8_t& c) {
		return std::tolower(c);
	});

	return data;
}

std::string ToUpper(const std::string& str)
{
	std::string data(str);
	std::transform(data.begin(), data.end(), data.begin(), [](const uint8_t& c) {
		return std::toupper(c);
	});

	return data;
}