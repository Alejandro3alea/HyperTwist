
namespace Math
{
    template<typename T>
    const T Lerp(const T& lhs, const T& rhs, const float t)
    {
        return lhs + (rhs - lhs) * t;
    }

    template<typename T>
    const T EaseIn(const T& lhs, const T& rhs, const float t)
    {
        const float easingFactor = t * t;
        return Lerp(lhs, rhs, easingFactor);
    }

    template<typename T>
    const T EaseOut(const T& lhs, const T& rhs, const float t)
    {
        const float easingFactor = t * (2 - t);
        return Lerp(lhs, rhs, easingFactor);
    }

    template<typename T>
    const T EaseInOut(const T& lhs, const T& rhs, const float t)
    {
        float easingFactor;
        if (t < 0.5f)
        {
            easingFactor = 2 * t * t;

        }
        else
        {
            easingFactor = -1 + (4 - 2 * t) * t;
        }
        return Lerp(lhs, rhs, easingFactor);
    }
}
