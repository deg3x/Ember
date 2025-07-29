internal quat_t
quat_normalize(quat_t* quat)
{
    f32_t inv_len = 1.0f / quat_length(quat);

    quat_t result = {
        quat->x * inv_len,
        quat->y * inv_len,
        quat->z * inv_len,
        quat->w * inv_len
    };

    return result;
}

internal quat_t
quat_renormalize(quat_t* quat)
{
    f32_t inv_len = math_fast_inv_sqrt_approx_one(quat_length_sqr(quat));

    quat_t result = {
        quat->x * inv_len,
        quat->y * inv_len,
        quat->z * inv_len,
        quat->w * inv_len
    };

    return result;
}

internal quat_t
quat_add(quat_t* lhs, quat_t* rhs)
{
    quat_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z,
        lhs->w + rhs->w
    };

    return result;
}

internal quat_t
quat_sub(quat_t* lhs, quat_t* rhs)
{
    quat_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z,
        lhs->w - rhs->w
    };

    return result;
}

internal quat_t
quat_mul(quat_t* lhs, quat_t* rhs)
{
    f32_t x = lhs->w * rhs->x + lhs->x * rhs->w + lhs->y * rhs->z - lhs->z * rhs->y;
    f32_t y = lhs->w * rhs->y + lhs->y * rhs->w - lhs->x * rhs->z + lhs->z * rhs->x;
    f32_t z = lhs->w * rhs->z + lhs->z * rhs->w + lhs->x * rhs->y - lhs->y * rhs->x;
    f32_t w = lhs->w * rhs->w - lhs->x * rhs->x - lhs->y * rhs->y - lhs->z * rhs->z;

    quat_t result = { x, y, z, w };

    return result;
}

internal quat_t
quat_mul_s(quat_t* lhs, f32_t rhs)
{
    quat_t result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs,
        lhs->w * rhs
    };

    return result;
}

internal quat_t
quat_inverse(quat_t* quat)
{
    f32_t inv_len_sqr = 1.0f / quat_length_sqr(quat);

    quat_t result = {
       -quat->x * inv_len_sqr,
       -quat->y * inv_len_sqr,
       -quat->z * inv_len_sqr,
        quat->w * inv_len_sqr
    };

    return result;
}

internal quat_t
quat_conjugate(quat_t* quat)
{
    quat_t result = {
       -quat->x,
       -quat->y,
       -quat->z,
        quat->w
    };

    return result;
}

internal f32_t
quat_dot(quat_t* lhs, quat_t* rhs)
{
    f32_t result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z +
        lhs->w * rhs->w;

    return result;
}

internal f32_t
quat_length(quat_t* quat)
{
    f32_t x_sqr = quat->x * quat->x;
    f32_t y_sqr = quat->y * quat->y;
    f32_t z_sqr = quat->z * quat->z;
    f32_t w_sqr = quat->w * quat->w;

    f32_t result = math_sqrt(x_sqr + y_sqr + z_sqr + w_sqr);

    return result;
}

internal f32_t
quat_length_sqr(quat_t* quat)
{
    f32_t result =
        quat->x * quat->x +
        quat->y * quat->y +
        quat->z * quat->z +
        quat->w * quat->w;

    return result;
}

internal vec3_t
quat_to_euler(quat_t* quat)
{
    vec3_t result;
    f32_t pitch_test = quat->w * quat->x + quat->y * quat->z;
    f32_t pitch_sign = math_sign(pitch_test);

    if (math_approx_equal(math_abs(pitch_test), 0.5f, MATH_EPSILON))
    {
        result.x = MATH_HALF_PI;
        result.y = pitch_sign * math_atan2(quat->y, quat->x);
        result.z = 0.0f;

        return result;
    }

    f32_t xx = quat->x * quat->x;
    f32_t yy = quat->y * quat->y;
    f32_t zz = quat->z * quat->z;
    f32_t xy = quat->x * quat->y;
    f32_t xz = quat->x * quat->z;
    f32_t wy = quat->w * quat->y;
    f32_t wz = quat->w * quat->z;

    f32_t pitch = math_atan2(2.0f * pitch_test, 1.0f - 2.0f * (xx + yy));
    f32_t yaw   = -math_asin(2.0f * (xz - wy));
    f32_t roll  = math_atan2(2.0f * (wz + xy), 1.0f - 2.0f * (yy + zz));

    result.x = pitch;
    result.y = yaw;
    result.z = roll;

    return result;
}

internal quat_t
quat_from_euler(vec3_t* angles)
{
    f32_t x_half = angles->x;
    f32_t y_half = angles->y;
    f32_t z_half = angles->z;
    f32_t cos_p  = math_cos(x_half);
    f32_t cos_y  = math_cos(y_half);
    f32_t cos_r  = math_cos(z_half);
    f32_t sin_p  = math_sin(x_half);
    f32_t sin_y  = math_sin(y_half);
    f32_t sin_r  = math_sin(z_half);

    f32_t x = sin_p * cos_y * cos_r - cos_p * sin_y * sin_r;
    f32_t y = cos_p * sin_y * cos_r + sin_p * cos_y * sin_r;
    f32_t z = cos_p * cos_y * sin_r - sin_p * sin_y * cos_r;
    f32_t w = cos_p * cos_y * cos_r + sin_p * sin_y * sin_r;

    quat_t result = { x, y, z, w };

    return result;
}

internal quat_t
quat_from_matrix(mat4_t* matrix)
{
    f32_t m00m11_a = matrix->m[0][0] + matrix->m[1][1];
    f32_t m00m11_s = matrix->m[0][0] - matrix->m[1][1];
    f32_t m22m33_a = matrix->m[2][2] + matrix->m[3][3];

    f32_t four_x_sqr = m00m11_s - m22m33_a;
    f32_t four_y_sqr = m00m11_a - m22m33_a;
    f32_t four_z_sqr = m00m11_s + m22m33_a;
    f32_t four_w_sqr = m00m11_a + m22m33_a;

    f32_t max_comp = four_w_sqr;
    u32_t max_idx  = 3;

    if (four_x_sqr > max_comp)
    {
        max_comp = four_x_sqr;
        max_idx  = 0;
    }
    if (four_y_sqr > max_comp)
    {
        max_comp = four_y_sqr;
        max_idx  = 1;
    }
    if (four_z_sqr > max_comp)
    {
        max_comp = four_z_sqr;
        max_idx  = 2;
    }

    f32_t ops_sign[4][3] = {
        {-1.0f,  1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
    };

    f32_t max_sqrt     = math_sqrt(max_comp);
    f32_t inv_two_sqrt = 1.0f / (2.0f * max_sqrt);

    quat_t result;
    result.q[max_idx]           = 0.5f * max_sqrt;
    result.q[(max_idx + 1) % 4] = (matrix->m[1][2] + ops_sign[max_idx][0] * matrix->m[2][1]) * inv_two_sqrt;
    result.q[(max_idx + 2) % 4] = (matrix->m[2][0] + ops_sign[max_idx][1] * matrix->m[0][2]) * inv_two_sqrt;
    result.q[(max_idx + 3) % 4] = (matrix->m[0][1] + ops_sign[max_idx][2] * matrix->m[1][0]) * inv_two_sqrt;

    return quat_normalize(&result);
}

internal quat_t
quat_from_axis_angle(vec3_t* axis, f32_t angle)
{
    f32_t half_angle     = angle * 0.5f;
    f32_t half_angle_cos = math_cos(half_angle);
    f32_t half_angle_sin = math_sin(half_angle);

    quat_t result = {
        half_angle_sin * axis->x,
        half_angle_sin * axis->y,
        half_angle_sin * axis->z,
        half_angle_cos
    };

    return result;
}

