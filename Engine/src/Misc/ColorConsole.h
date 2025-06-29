#pragma once
#include <iostream>
#include <format>

#ifdef _WIN32
	#include <Windows.h>
#endif

#undef Time

enum ConsoleColor
{
	Default,
	Red,
	Green,
	Yellow,
	Blue,
	Purple
};

void PrintColoredText(const std::string& text, const ConsoleColor color);

void PrintDebug(const std::string& text);
void PrintSuccess(const std::string& text);
void PrintWarning(const std::string& text);
void PrintError(const std::string& text);


// God bless The Cherno for opening my eyes
template <typename... Args>
void PrintDebug(const std::format_string<Args...> text, Args&&... args) { PrintDebug(std::format(text, std::forward<Args>(args)...)); }
template <typename... Args>
void PrintSuccess(const std::format_string<Args...> text, Args&&... args) { PrintSuccess(std::format(text, std::forward<Args>(args)...)); }
template <typename... Args>
void PrintWarning(const std::format_string<Args...> text, Args&&... args) { PrintWarning(std::format(text, std::forward<Args>(args)...)); }
template <typename... Args>
void PrintError(const std::format_string<Args...> text, Args&&... args) { PrintError(std::format(text, std::forward<Args>(args)...)); }