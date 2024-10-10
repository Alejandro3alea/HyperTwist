#pragma once
#include "Graphics/Renderable.h"

#include <string>
#include <vector>
#include <iostream>
#include <functional>

struct DevMenuItem
{
public:
    DevMenuItem(const std::string& name) : mName(name) {}
    virtual ~DevMenuItem() {}

    virtual void OnSelected() = 0;

    std::string GetName() const { return mName; }

protected:
    std::string mName;
};