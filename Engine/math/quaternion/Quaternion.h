#pragma once

#include "Definitions.h"
#include "math/vector/Vector3.h"

struct Matrix4x4;

struct ENGINE_API Quaternion
{
    real w;
    real x;
    real y;
    real z;

    Quaternion() : w(1.0), x(0.0), y(0.0), z(0.0) {}
    Quaternion(real w, real x, real y, real z) : w(w), x(x), y(y), z(z) {}
    explicit Quaternion(const Vector3& vector);
    Quaternion(const Vector3& axis, real angle);
    Quaternion(const Matrix4x4& matrix);

    Quaternion& Normalize();
    Quaternion& Renormalize();
    Vector3 RotateVector(const Vector3& vector) const;

    static real Dot(const Quaternion& lhs, const Quaternion& rhs);
    static real Length(const Quaternion& quat);
    static real LengthSqr(const Quaternion& quat);
    static Quaternion Inverse(const Quaternion& quat);
    static Quaternion Normalize(const Quaternion& quat);
    static Quaternion Renormalize(const Quaternion& quat);
    static Quaternion Conjugate(const Quaternion& quat);
    
    Quaternion& operator+=(const Quaternion& rhs);
    Quaternion& operator-=(const Quaternion& rhs);
    Quaternion& operator*=(const Quaternion& rhs);
    Quaternion& operator/=(const Quaternion& rhs);
    
    Quaternion& operator+=(real rhs);
    Quaternion& operator-=(real rhs);
    Quaternion& operator*=(real rhs);
    Quaternion& operator/=(real rhs);
};

inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion ret = lhs;

    ret += rhs;
    
    return ret;
}

inline Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion ret = lhs;

    ret -= rhs;
    
    return ret;
}

inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion ret = lhs;

    ret *= rhs;
    
    return ret;
}

inline Quaternion operator/(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion ret = lhs;

    ret /= rhs;
    
    return ret;
}

inline Vector3 operator*(const Quaternion& lhs, const Vector3& rhs)
{
    const Vector3 ret = Vector3(lhs * Quaternion(rhs) * Quaternion::Inverse(lhs));

    return ret;
}

inline Quaternion operator*(const Quaternion& lhs, real rhs)
{
    Quaternion ret = lhs;

    ret *= rhs;
    
    return ret;
}

inline Quaternion operator*(real lhs, const Quaternion& rhs)
{
    Quaternion ret = rhs;

    ret *= lhs;
    
    return ret;
}

inline Quaternion operator/(const Quaternion& lhs, real rhs)
{
    Quaternion ret = lhs;

    ret /= rhs;
    
    return ret;
}

inline Quaternion operator/(real lhs, const Quaternion& rhs)
{
    Quaternion ret;

    ret.w = lhs / rhs.w;
    ret.x = lhs / rhs.x;
    ret.y = lhs / rhs.y;
    ret.z = lhs / rhs.z;
    
    return ret;
}