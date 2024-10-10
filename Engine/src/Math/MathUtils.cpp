#include "MathUtils.h"
#include <cmath>

namespace Math
{
	std::map<int, int> GetPrimeFactors(int num)
    {
        std::map<int, int> factors;
        for (int i = 2; i <= num; ++i)
        {
            while (num % i == 0)
            {
                factors[i]++;
                num /= i;
            }
        }
        return factors;
    }

    float GetFactorial(const float number)
    {
        const float result = number - std::floor(number);
        return (result != 0.0f) ? result : 1.0f;
    }
}