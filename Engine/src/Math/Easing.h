#pragma once
#include "Composition/Transform.h"

namespace Math
{
    template<typename T>
    const T Lerp(const T& lhs, const T& rhs, const float easingFactor);
    inline Transform Lerp(const Transform& lhs, const Transform& rhs, float easingFactor)
    {
        return { Lerp(lhs.pos, rhs.pos, easingFactor), Lerp(lhs.scale, rhs.scale, easingFactor), Lerp(lhs.rotation, rhs.rotation, easingFactor) };
    }

    template<typename T>
    const T EaseIn(const T& lhs, const T& rhs, const float easingFactor);
    inline Transform EaseIn(const Transform& lhs, const Transform& rhs, float easingFactor)
    {
        return { EaseIn(lhs.pos, rhs.pos, easingFactor), EaseIn(lhs.scale, rhs.scale, easingFactor), EaseIn(lhs.rotation, rhs.rotation, easingFactor) };
    }

    template<typename T>
    const T EaseOut(const T& lhs, const T& rhs, const float easingFactor);
    inline Transform EaseOut(const Transform& lhs, const Transform& rhs, float easingFactor)
    {
        return { EaseOut(lhs.pos, rhs.pos, easingFactor), EaseOut(lhs.scale, rhs.scale, easingFactor), EaseOut(lhs.rotation, rhs.rotation, easingFactor) };
    }

    template<typename T>
    const T EaseInOut(const T& lhs, const T& rhs, const float easingFactor);
    inline Transform EaseInOut(const Transform& lhs, const Transform& rhs, float easingFactor)
    {
        return { EaseInOut(lhs.pos, rhs.pos, easingFactor), EaseInOut(lhs.scale, rhs.scale, easingFactor), EaseInOut(lhs.rotation, rhs.rotation, easingFactor) };
    }
}

#include "Easing.inl"