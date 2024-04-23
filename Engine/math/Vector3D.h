#pragma once

#include "Definitions.h"

struct ENGINE_API Vector3D
{
    float x;
    float y;
    float z;

    static float Dot(const Vector3D& lhs, const Vector3D& rhs);
    static float Length(const Vector3D& vector);
    static float LengthSqr(const Vector3D& vector);
    static Vector3D Cross(const Vector3D& lhs, const Vector3D& rhs);
    static Vector3D Normalize(const Vector3D& vector);
    
    Vector3D& operator+=(const Vector3D& rhs);
    Vector3D& operator-=(const Vector3D& rhs);

    Vector3D& operator*=(float rhs);
    Vector3D& operator/=(float rhs);
};

inline Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs)
{
    Vector3D ret = lhs;

    ret += rhs;
    
    return ret;
}

inline Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs)
{
    Vector3D ret = lhs;

    ret -= rhs;
    
    return ret;
}

inline Vector3D operator/(const Vector3D& lhs, float rhs)
{
    Vector3D ret = lhs;

    ret /= rhs;
    
    return ret;
}

inline Vector3D operator/(float lhs, const Vector3D& rhs)
{
    Vector3D ret;

    ret.x = lhs / rhs.x;
    ret.y = lhs / rhs.y;
    ret.z = lhs / rhs.z;
    
    return ret;
}