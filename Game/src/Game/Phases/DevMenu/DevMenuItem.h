#pragma once
#include "Graphics/Renderables/Renderable.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>

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