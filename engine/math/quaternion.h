#ifndef QUATERNION_H
#define QUATERNION_H

typedef union quat_t quat_t;
union quat_t
{
    struct
    {
        f32_t x;
        f32_t y;
        f32_t z;
        f32_t w;
    };

    f32_t q[4];
};

internal quat_t quat_normalize(quat_t* quat);
internal quat_t quat_renormalize(quat_t* quat);

internal quat_t quat_add(quat_t* lhs, quat_t* rhs);
internal quat_t quat_sub(quat_t* lhs, quat_t* rhs);
internal quat_t quat_mul(quat_t* lhs, quat_t* rhs);
internal quat_t quat_mul_s(quat_t* lhs, f32_t rhs);

internal quat_t quat_inverse(quat_t* quat);
internal quat_t quat_conjugate(quat_t* quat);

internal f32_t quat_dot(quat_t* lhs, quat_t* rhs);
internal f32_t quat_length(quat_t* quat);
internal f32_t quat_length_sqr(quat_t* quat);

internal vec3_t quat_to_euler(quat_t* quat);
internal quat_t quat_from_euler(vec3_t* angles);
internal quat_t quat_from_matrix(mat4_t* matrix);
internal quat_t quat_from_axis_angle(vec3_t* axis, f32_t angle);

#endif
