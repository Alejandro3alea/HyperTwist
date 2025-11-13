#include "ColorConsole.h"
#include "Utils/Config.h"

void PrintColoredText(const std::string& text, const ConsoleColor color)
{
#ifdef _WIN32

    int col = 15;
    switch (color)
    {
    case Red:
        col = 12;
        break;
    case Green:
        col = 10;
        break;
    case Yellow:
        col = 6;
        break;
    case Blue:
        col = 3;
        break;
    case Purple:
        col = 5;
        break;
    default:
        break;
    }

    const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, col);
    std::cout << text << std::endl;
    SetConsoleTextAttribute(hConsole, 15);

#else // Linux/macOS

    const char* code = "\033[0m";
    switch (color)
    {
    case Red:
        code = "\033[31m";
        break;
    case Green:
        code = "\033[32m";
        break;
    case Yellow:
        code = "\033[33m";
        break;
    case Blue:
        code = "\033[34m";
        break;
    case Purple:
        code = "\033[35m";
        break;
    default:
        code = "\033[0m";
        break;
    }

    std::cout << code << text << "\033[0m" << std::endl;

#endif
}

void PrintSuccess(const std::string& text) { PrintColoredText(text, ConsoleColor::Green); }

void PrintDebug(const std::string& text)
{
#if DEBUG_BUILD
    PrintColoredText("[DEBUG] - " + text, ConsoleColor::Blue);
#endif
}

void PrintWarning(const std::string& text) { PrintColoredText("[WARNING] - " + text, ConsoleColor::Yellow); }

void PrintError(const std::string& text) { PrintColoredText("[ERROR] - " + text, ConsoleColor::Red); }