#pragma once

namespace Math
{
    template<typename T>
    const T Lerp(const T& lhs, const T& rhs, const float easingFactor);

    template<typename T>
    const T EaseIn(const T& lhs, const T& rhs, const float easingFactor);

    template<typename T>
    const T EaseOut(const T& lhs, const T& rhs, const float easingFactor);

    template<typename T>
    const T EaseInOut(const T& lhs, const T& rhs, const float easingFactor);
}

#include "Easing.inl"