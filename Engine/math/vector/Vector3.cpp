#include "engine_pch.h"
#include "Vector3.h"

#include "math/Math.h"
#include "math/quaternion/Quaternion.h"

Vector3::Vector3(const Quaternion& quat)
{
    x = quat.x;
    y = quat.y;
    z = quat.z;
}

real Vector3::Length() const
{
    return Sqrt(x * x + y * y + z * z);
}

real Vector3::LengthSqr() const
{
    return x * x + y * y + z * z;
}

Vector3 Vector3::Normalize() const
{
    const real invLength = static_cast<real>(1.0) / Length(*this);

    return *this * invLength;
}

bool Vector3::IsEqual(const Vector3& vector, real error) const
{
    return ApproxEqual(x, vector.x, error)
        && ApproxEqual(y, vector.y, error)
        && ApproxEqual(z, vector.z, error);
}

bool Vector3::IsZero(real error) const
{
    return ApproxZero(x, error)
        && ApproxZero(y, error)
        && ApproxZero(z, error);
}

Vector3 Vector3::Rotate(const Vector3& axis, real angle) const
{
    const Quaternion quat = Quaternion(axis, angle);

    return quat.RotateVector(*this);
}

std::string Vector3::ToString() const
{
    return "{ "
        + std::to_string(x) + ", "
        + std::to_string(y) + ", "
        + std::to_string(z) + " }";
}

real Vector3::Dot(const Vector3& lhs, const Vector3& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

real Vector3::Length(const Vector3& vector)
{
    return Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

real Vector3::LengthSqr(const Vector3& vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

Vector3 Vector3::Cross(const Vector3& lhs, const Vector3& rhs)
{
    Vector3 ret;

    ret.x = lhs.y * rhs.z - rhs.y * lhs.z;
    ret.y = rhs.x * lhs.z - lhs.x * rhs.z;
    ret.z = lhs.x * rhs.y - rhs.x * lhs.y;

    return ret;
}

Vector3 Vector3::Normalize(const Vector3& vector)
{
    return vector / Length(vector);
}

Vector3 Vector3::operator-() const
{
    return { -x, -y, -z };
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;

    return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;

    return *this;
}

Vector3& Vector3::operator*=(const Vector3& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;

    return *this;
}

Vector3& Vector3::operator*=(real rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;

    return *this;
}

Vector3& Vector3::operator/=(real rhs)
{
    const real invRhs = static_cast<real>(1.0) / rhs;
    
    *this *= invRhs;

    return *this;
}

real& Vector3::operator[](int idx)
{
    return *(&x + (idx % 3));
}

const real& Vector3::operator[](int idx) const
{
    return *(&x + (idx % 3));
}