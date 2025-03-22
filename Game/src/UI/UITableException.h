#pragma once
#include <exception>
#include <string>
#include <utility>

struct UITableException final : public std::exception
{
    explicit UITableException(std::string what) : reason(std::move(what)) {}
    
public:
    const std::string reason;
};