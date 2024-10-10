#pragma once
#include "Easing.h"

#include <map>

namespace Math
{
    std::map<int, int> GetPrimeFactors(int num);

    float GetFactorial(const float number);

    template<typename T>
    const T Clamp(const T& value, const T& minVal, const T& maxVal) { return (value < minVal) ? minVal : (value > maxVal) ? maxVal : value; }
}