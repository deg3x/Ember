#include "engine_pch.h"
#include "Math.h"

real Sqrt(real value)
{
    // Shameless implementation
    // Change in the future
    return glm::sqrt(value);
}

real FastInvSqrtApproxOne(real value)
{
    constexpr real a =  1.875f;
    constexpr real b = -1.250f;
    constexpr real c =  0.375f;

    return a + b * value + c * value * value;
}

real Sin(real value)
{
    return glm::sin(value);
}

real Cos(real value)
{
    return glm::cos(value);
}

real Asin(real value)
{
    return glm::asin(value);
}

real Acos(real value)
{
    return glm::acos(value);
}

bool ApproxZero(real value)
{
    return (value > -FLOAT_MIN && value < FLOAT_MIN);
}