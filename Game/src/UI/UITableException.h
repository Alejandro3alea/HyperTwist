#pragma once
#include <exception>
#include <string>
#include <utility>

struct UITableException final : public std::exception
{
    explicit UITableException(std::string what) : reason(std::move(what)) {}
    _NODISCARD char const* what() const override {
        return reason.c_str();
    }

public:
    const std::string reason;
};