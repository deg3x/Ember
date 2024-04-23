#pragma once

#include "Definitions.h"

struct ENGINE_API Vector3
{
    real x;
    real y;
    real z;

    Vector3& Normalize();

    static real Dot(const Vector3& lhs, const Vector3& rhs);
    static real Length(const Vector3& vector);
    static real LengthSqr(const Vector3& vector);
    static Vector3 Cross(const Vector3& lhs, const Vector3& rhs);
    static Vector3 Normalize(const Vector3& vector);
    
    Vector3& operator+=(const Vector3& rhs);
    Vector3& operator-=(const Vector3& rhs);

    Vector3& operator*=(real rhs);
    Vector3& operator/=(real rhs);
};

inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
{
    Vector3 ret = lhs;

    ret += rhs;
    
    return ret;
}

inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
    Vector3 ret = lhs;

    ret -= rhs;
    
    return ret;
}

inline Vector3 operator/(const Vector3& lhs, real rhs)
{
    Vector3 ret = lhs;

    ret /= rhs;
    
    return ret;
}

inline Vector3 operator/(real lhs, const Vector3& rhs)
{
    Vector3 ret;

    ret.x = lhs / rhs.x;
    ret.y = lhs / rhs.y;
    ret.z = lhs / rhs.z;
    
    return ret;
}