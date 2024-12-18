#pragma once

#include "Definitions.h"

struct ENGINE_API Vector2
{
    real x;
    real y;

    constexpr Vector2() : x(0.0), y(0.0) {}
    constexpr Vector2(real x, real y) : x(x), y(y) {}
    
    Vector2 Normalize() const;
    
    bool IsEqual(const Vector2& vector, real error = EPSILON) const;
    bool IsZero(real error = EPSILON) const;
    
    std::string ToString() const;

    static real Dot(const Vector2& lhs, const Vector2& rhs);
    static real Length(const Vector2& vector);
    static real LengthSqr(const Vector2& vector);
    static Vector2 Normalize(const Vector2& vector);
    
    Vector2 operator-() const;
    
    Vector2& operator+=(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);
    Vector2& operator*=(const Vector2& rhs);

    Vector2& operator*=(real rhs);
    Vector2& operator/=(real rhs);

    real& operator[](int idx);
    const real& operator[](int idx) const;
};

inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
    Vector2 ret = lhs;

    ret += rhs;
    
    return ret;
}

inline Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
    Vector2 ret = lhs;

    ret -= rhs;
    
    return ret;
}

inline Vector2 operator*(const Vector2 lhs, const Vector2 rhs)
{
    Vector2 ret = lhs;

    ret *= rhs;
    
    return ret;
}

inline Vector2 operator*(const Vector2& lhs, real rhs)
{
    Vector2 ret = lhs;

    ret *= rhs;
    
    return ret;
}

inline Vector2 operator*(real lhs, const Vector2& rhs)
{
    Vector2 ret = rhs;

    ret *= lhs;
    
    return ret;
}

inline Vector2 operator/(const Vector2& lhs, real rhs)
{
    Vector2 ret = lhs;

    ret /= rhs;
    
    return ret;
}

inline Vector2 operator/(real lhs, const Vector2& rhs)
{
    Vector2 ret;

    ret.x = lhs / rhs.x;
    ret.y = lhs / rhs.y;
    
    return ret;
}
