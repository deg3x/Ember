#include "engine_pch.h"
#include "Matrix4x4.h"

Matrix4x4::Matrix4x4()
{
    values[0] = Vector4(1.0, 0.0, 0.0, 0.0);
    values[1] = Vector4(0.0, 1.0, 0.0, 0.0);
    values[2] = Vector4(0.0, 0.0, 1.0, 0.0);
    values[3] = Vector4(0.0, 0.0, 0.0, 1.0);
}

real Matrix4x4::Determinant() const
{
    return 0.0;
}

Matrix4x4 Matrix4x4::Inverse() const
{
    return Matrix4x4();
}

Matrix4x4 Matrix4x4::Transpose() const
{
    Matrix4x4 transpose;

    transpose[0][0] = (*this)[0][0];
    transpose[0][1] = (*this)[1][0];
    transpose[0][2] = (*this)[2][0];
    transpose[0][3] = (*this)[3][0];

    transpose[1][0] = (*this)[0][1];
    transpose[1][1] = (*this)[1][1];
    transpose[1][2] = (*this)[2][1];
    transpose[1][3] = (*this)[3][1];

    transpose[2][0] = (*this)[0][2];
    transpose[2][1] = (*this)[1][2];
    transpose[2][2] = (*this)[2][2];
    transpose[2][3] = (*this)[3][2];

    transpose[3][0] = (*this)[0][3];
    transpose[3][1] = (*this)[1][3];
    transpose[3][2] = (*this)[2][3];
    transpose[3][3] = (*this)[3][3];
    
    return transpose;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& rhs)
{
    values[0] += rhs[0];
    values[1] += rhs[1];
    values[2] += rhs[2];
    values[3] += rhs[3];
    
    return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& rhs)
{
    values[0] -= rhs[0];
    values[1] -= rhs[1];
    values[2] -= rhs[2];
    values[3] -= rhs[3];
    
    return *this;
}

// Very unoptimized version of multiplication
// Improve in the future
Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs)
{
    const Matrix4x4 transpose = this->Transpose();

#define dot Vector4::Dot
    values[0] = Vector4(dot(transpose[0], rhs[0]), dot(transpose[1], rhs[0]), dot(transpose[2], rhs[0]), dot(transpose[3], rhs[0]));
    values[1] = Vector4(dot(transpose[0], rhs[1]), dot(transpose[1], rhs[1]), dot(transpose[2], rhs[1]), dot(transpose[3], rhs[1]));
    values[2] = Vector4(dot(transpose[0], rhs[2]), dot(transpose[1], rhs[2]), dot(transpose[2], rhs[2]), dot(transpose[3], rhs[2]));
    values[3] = Vector4(dot(transpose[0], rhs[3]), dot(transpose[1], rhs[3]), dot(transpose[2], rhs[3]), dot(transpose[3], rhs[3]));
#undef dot
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(real rhs)
{
    values[0] *= rhs;
    values[1] *= rhs;
    values[2] *= rhs;
    values[3] *= rhs;
    
    return *this;
}

Matrix4x4& Matrix4x4::operator/=(real rhs)
{
    const real invRhs = static_cast<real>(1.0) / rhs;
    
    values[0] *= invRhs;
    values[1] *= invRhs;
    values[2] *= invRhs;
    values[3] *= invRhs;
    
    return *this;
}

Vector4& Matrix4x4::operator[](int idx)
{
    return values[idx];
}

const Vector4& Matrix4x4::operator[](int idx) const
{
    return values[idx];
}
