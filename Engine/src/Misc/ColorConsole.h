#pragma once
#include <Windows.h>
#include <iostream>

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