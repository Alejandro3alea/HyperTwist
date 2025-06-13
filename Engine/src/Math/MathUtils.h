#pragma once
#include "Easing.h"
#include "Misc/Concepts.h"

#include <map>

namespace Math
{
    std::map<int, int> GetPrimeFactors(int num);

    float GetFactorial(const float number);

    template<Arithmetic T>
    constexpr T Clamp(const T& value, const T& minVal, const T& maxVal) { return (value < minVal) ? minVal : (value > maxVal) ? maxVal : value; }

    template<Arithmetic T>
    constexpr T Remap(const T& value, const T& oldMin, const T& oldMax, const T& newMin, const T& newMax)
    {
        if (oldMin == oldMax)
            throw std::invalid_argument("Remap(): oldMin and oldMax must differ.");

        return (value - oldMin) * (newMax - newMin) / (oldMax - oldMin) + newMin;

    }
}