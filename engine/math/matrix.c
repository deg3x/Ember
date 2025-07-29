internal f32_t
mat4_determinant(mat4_t* matrix)
{
    f32_t det_23 = matrix->m[2][2] * matrix->m[3][3] - matrix->m[2][3] * matrix->m[3][2];
    f32_t det_13 = matrix->m[1][2] * matrix->m[3][3] - matrix->m[1][3] * matrix->m[3][2];
    f32_t det_12 = matrix->m[1][2] * matrix->m[2][3] - matrix->m[1][3] * matrix->m[2][2];
    f32_t det_03 = matrix->m[0][2] * matrix->m[3][3] - matrix->m[0][3] * matrix->m[3][2];
    f32_t det_02 = matrix->m[0][2] * matrix->m[2][3] - matrix->m[0][3] * matrix->m[2][2];
    f32_t det_01 = matrix->m[0][2] * matrix->m[1][3] - matrix->m[0][3] * matrix->m[1][2];

    f32_t det_123 = matrix->m[1][1] * det_23 - matrix->m[2][1] * det_13 + matrix->m[3][1] * det_12;
    f32_t det_023 = matrix->m[0][1] * det_23 - matrix->m[2][1] * det_03 + matrix->m[3][1] * det_02;
    f32_t det_013 = matrix->m[0][1] * det_13 - matrix->m[1][1] * det_03 + matrix->m[3][1] * det_01;
    f32_t det_012 = matrix->m[0][1] * det_12 - matrix->m[1][1] * det_02 + matrix->m[2][1] * det_01;

    f32_t result =
        matrix->m[0][0] * det_123 -
        matrix->m[1][0] * det_023 +
        matrix->m[2][0] * det_013 -
        matrix->m[3][0] * det_012;

    return result;
}

internal mat4_t
mat4_add(mat4_t* lhs, mat4_t* rhs)
{
    mat4_t result = {{
        vec4_add(&lhs->cols[0], &rhs->cols[0]),
        vec4_add(&lhs->cols[1], &rhs->cols[1]),
        vec4_add(&lhs->cols[2], &rhs->cols[2]),
        vec4_add(&lhs->cols[3], &rhs->cols[3]),
    }};

    return result;
}

internal mat4_t
mat4_sub(mat4_t* lhs, mat4_t* rhs)
{
    mat4_t result = {{
        vec4_sub(&lhs->cols[0], &rhs->cols[0]),
        vec4_sub(&lhs->cols[1], &rhs->cols[1]),
        vec4_sub(&lhs->cols[2], &rhs->cols[2]),
        vec4_sub(&lhs->cols[3], &rhs->cols[3]),
    }};

    return result;
}

internal mat4_t
mat4_mul(mat4_t* lhs, mat4_t* rhs)
{
    mat4_t lhs_tr = mat4_transpose(lhs);

    mat4_t result = {
        .m = {
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[0]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[0]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[0]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[0])},
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[1]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[1]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[1]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[1])},
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[2]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[2]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[2]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[2])},
            {vec4_dot(&lhs_tr.cols[0], &rhs->cols[3]), vec4_dot(&lhs_tr.cols[1], &rhs->cols[3]), vec4_dot(&lhs_tr.cols[2], &rhs->cols[3]), vec4_dot(&lhs_tr.cols[3], &rhs->cols[3])},
        }
    };

    return result;
}

internal mat4_t
mat4_mul_s(mat4_t* lhs, f32_t rhs)
{
    mat4_t result = {{
        vec4_mul_s(&lhs->cols[0], rhs),
        vec4_mul_s(&lhs->cols[1], rhs),
        vec4_mul_s(&lhs->cols[2], rhs),
        vec4_mul_s(&lhs->cols[3], rhs),
    }};

    return result;
}

internal mat4_t
mat4_inverse(mat4_t* matrix)
{
    f32_t det_23_23 = matrix->m[2][2] * matrix->m[3][3] - matrix->m[2][3] * matrix->m[3][2];
    f32_t det_23_13 = matrix->m[1][2] * matrix->m[3][3] - matrix->m[1][3] * matrix->m[3][2];
    f32_t det_23_12 = matrix->m[1][2] * matrix->m[2][3] - matrix->m[1][3] * matrix->m[2][2];
    f32_t det_23_03 = matrix->m[0][2] * matrix->m[3][3] - matrix->m[0][3] * matrix->m[3][2];
    f32_t det_23_02 = matrix->m[0][2] * matrix->m[2][3] - matrix->m[0][3] * matrix->m[2][2];
    f32_t det_23_01 = matrix->m[0][2] * matrix->m[1][3] - matrix->m[0][3] * matrix->m[1][2];

    f32_t det_123_123 = matrix->m[1][1] * det_23_23 - matrix->m[2][1] * det_23_13 + matrix->m[3][1] * det_23_12;
    f32_t det_123_023 = matrix->m[0][1] * det_23_23 - matrix->m[2][1] * det_23_03 + matrix->m[3][1] * det_23_02;
    f32_t det_123_013 = matrix->m[0][1] * det_23_13 - matrix->m[1][1] * det_23_03 + matrix->m[3][1] * det_23_01;
    f32_t det_123_012 = matrix->m[0][1] * det_23_12 - matrix->m[1][1] * det_23_02 + matrix->m[2][1] * det_23_01;

    f32_t determinant =
        matrix->m[0][0] * det_123_123 -
        matrix->m[1][0] * det_123_023 +
        matrix->m[2][0] * det_123_013 -
        matrix->m[3][0] * det_123_012;

    EMBER_ASSERT(!math_approx_zero(determinant));

    f32_t det_13_23 = matrix->m[2][1] * matrix->m[3][3] - matrix->m[2][3] * matrix->m[3][1];
    f32_t det_13_13 = matrix->m[1][1] * matrix->m[3][3] - matrix->m[1][3] * matrix->m[3][1];
    f32_t det_13_12 = matrix->m[1][1] * matrix->m[2][3] - matrix->m[1][3] * matrix->m[2][1];
    f32_t det_13_03 = matrix->m[0][1] * matrix->m[3][3] - matrix->m[0][3] * matrix->m[3][1];
    f32_t det_13_02 = matrix->m[0][1] * matrix->m[2][3] - matrix->m[0][3] * matrix->m[2][1];
    f32_t det_13_01 = matrix->m[0][1] * matrix->m[1][3] - matrix->m[0][3] * matrix->m[1][1];

    f32_t det_12_23 = matrix->m[2][1] * matrix->m[3][2] - matrix->m[2][2] * matrix->m[3][1];
    f32_t det_12_13 = matrix->m[1][1] * matrix->m[3][2] - matrix->m[1][2] * matrix->m[3][1];
    f32_t det_12_12 = matrix->m[1][1] * matrix->m[2][2] - matrix->m[1][2] * matrix->m[2][1];
    f32_t det_12_03 = matrix->m[0][1] * matrix->m[3][2] - matrix->m[0][2] * matrix->m[3][1];
    f32_t det_12_02 = matrix->m[0][1] * matrix->m[2][2] - matrix->m[0][2] * matrix->m[2][1];
    f32_t det_12_01 = matrix->m[0][1] * matrix->m[1][2] - matrix->m[0][2] * matrix->m[1][1];

    f32_t det_023_123 = matrix->m[1][0] * det_23_23 - matrix->m[2][0] * det_23_13 + matrix->m[3][0] * det_23_12;
    f32_t det_023_023 = matrix->m[0][0] * det_23_23 - matrix->m[2][0] * det_23_03 + matrix->m[3][0] * det_23_02;
    f32_t det_023_013 = matrix->m[0][0] * det_23_13 - matrix->m[1][0] * det_23_03 + matrix->m[3][0] * det_23_01;
    f32_t det_023_012 = matrix->m[0][0] * det_23_12 - matrix->m[1][0] * det_23_02 + matrix->m[2][0] * det_23_01;

    f32_t det_013_123 = matrix->m[1][0] * det_13_23 - matrix->m[2][0] * det_13_13 + matrix->m[3][0] * det_13_12;
    f32_t det_013_023 = matrix->m[0][0] * det_13_23 - matrix->m[2][0] * det_13_03 + matrix->m[3][0] * det_13_02;
    f32_t det_013_013 = matrix->m[0][0] * det_13_13 - matrix->m[1][0] * det_13_03 + matrix->m[3][0] * det_13_01;
    f32_t det_013_012 = matrix->m[0][0] * det_13_12 - matrix->m[1][0] * det_13_02 + matrix->m[2][0] * det_13_01;

    f32_t det_012_123 = matrix->m[1][0] * det_12_23 - matrix->m[2][0] * det_12_13 + matrix->m[3][0] * det_12_12;
    f32_t det_012_023 = matrix->m[0][0] * det_12_23 - matrix->m[2][0] * det_12_03 + matrix->m[3][0] * det_12_02;
    f32_t det_012_013 = matrix->m[0][0] * det_12_13 - matrix->m[1][0] * det_12_03 + matrix->m[3][0] * det_12_01;
    f32_t det_012_012 = matrix->m[0][0] * det_12_12 - matrix->m[1][0] * det_12_02 + matrix->m[2][0] * det_12_01;

    f32_t inv_det = 1.0f / determinant;

    mat4_t result;

    result.m[0][0] =  det_123_123 * inv_det;
    result.m[0][1] = -det_123_023 * inv_det;
    result.m[0][2] =  det_123_013 * inv_det;
    result.m[0][3] = -det_123_012 * inv_det;

    result.m[1][0] = -det_023_123 * inv_det;
    result.m[1][1] =  det_023_023 * inv_det;
    result.m[1][2] = -det_023_013 * inv_det;
    result.m[1][3] =  det_023_012 * inv_det;

    result.m[2][0] =  det_013_123 * inv_det;
    result.m[2][1] = -det_013_023 * inv_det;
    result.m[2][2] =  det_013_013 * inv_det;
    result.m[2][3] = -det_013_012 * inv_det;

    result.m[3][0] = -det_012_123 * inv_det;
    result.m[3][1] =  det_012_023 * inv_det;
    result.m[3][2] = -det_012_013 * inv_det;
    result.m[3][3] =  det_012_012 * inv_det;

    return result;
}

internal mat4_t
mat4_transpose(mat4_t* matrix)
{
    mat4_t result;

    result.m[0][0] = matrix->m[0][0];
    result.m[0][1] = matrix->m[1][0];
    result.m[0][2] = matrix->m[2][0];
    result.m[0][3] = matrix->m[3][0];

    result.m[1][0] = matrix->m[0][1];
    result.m[1][1] = matrix->m[1][1];
    result.m[1][2] = matrix->m[2][1];
    result.m[1][3] = matrix->m[3][1];

    result.m[2][0] = matrix->m[0][2];
    result.m[2][1] = matrix->m[1][2];
    result.m[2][2] = matrix->m[2][2];
    result.m[2][3] = matrix->m[3][2];

    result.m[3][0] = matrix->m[0][3];
    result.m[3][1] = matrix->m[1][3];
    result.m[3][2] = matrix->m[2][3];
    result.m[3][3] = matrix->m[3][3];

    return result;
}


internal vec3_t
mat4_to_euler(mat4_t* matrix)
{
    f32_t pitch =  math_atan2(matrix->m[1][2], matrix->m[2][2]);
    f32_t roll  =  math_atan2(matrix->m[0][1], matrix->m[0][0]);
    f32_t yaw   = -math_asin(matrix->m[0][2]);

    vec3_t result = { pitch, yaw, roll };

    return result;
}

internal mat4_t
mat4_from_euler(vec3_t* angles)
{
    f32_t cos_x = math_cos(angles->x);
    f32_t cos_y = math_cos(angles->y);
    f32_t cos_z = math_cos(angles->z);
    f32_t sin_x = math_sin(angles->x);
    f32_t sin_y = math_sin(angles->y);
    f32_t sin_z = math_sin(angles->z);

    mat4_t result = {};

    result.m[0][0] =  cos_y * cos_z;
    result.m[0][1] =  sin_z * cos_y;
    result.m[0][2] = -sin_y;

    result.m[1][0] =  sin_y * sin_x * cos_z - cos_x * sin_z;
    result.m[1][1] =  sin_y * sin_x * sin_z + cos_x * cos_z;
    result.m[1][2] =  cos_y * sin_x;

    result.m[2][0] =  sin_y * cos_x * cos_z + sin_x * sin_z;
    result.m[2][1] =  sin_y * cos_x * sin_z - sin_x * cos_z;
    result.m[2][2] =  cos_y * cos_x;

    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t
mat4_from_pitch(f32_t pitch)
{
    mat4_t result = {};

    f32_t angle_cos = math_cos(pitch);
    f32_t angle_sin = math_sin(pitch);

    result.m[1][1] =  angle_cos;
    result.m[1][2] =  angle_sin;
    result.m[2][1] = -angle_sin;
    result.m[2][2] =  angle_cos;
    result.m[0][0] =  1.0f;
    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t
mat4_from_yaw(f32_t yaw)
{
    mat4_t result = {};

    f32_t angle_cos = math_cos(yaw);
    f32_t angle_sin = math_sin(yaw);

    result.m[0][0] =  angle_cos;
    result.m[0][2] = -angle_sin;
    result.m[2][0] =  angle_sin;
    result.m[2][2] =  angle_cos;
    result.m[1][1] =  1.0f;
    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t
mat4_from_roll(f32_t roll)
{
    mat4_t result = {};

    f32_t angle_cos = math_cos(roll);
    f32_t angle_sin = math_sin(roll);

    result.m[0][0] =  angle_cos;
    result.m[0][1] =  angle_sin;
    result.m[1][0] = -angle_sin;
    result.m[1][1] =  angle_cos;
    result.m[2][2] =  1.0f;
    result.m[3][3] =  1.0f;

    return result;
}

internal mat4_t
mat4_from_diagonal(f32_t diagonal)
{
    mat4_t result = {
        .m = {
            { diagonal, 0.0f, 0.0f, 0.0f },
            { 0.0f, diagonal, 0.0f, 0.0f },
            { 0.0f, 0.0f, diagonal, 0.0f },
            { 0.0f, 0.0f, 0.0f, diagonal }
        }
    };

    return result;
}

internal mat4_t
mat4_from_quaternion(quat_t* quat)
{
    mat4_t result;

    f32_t ww = quat->w * quat->w;
    f32_t wx = quat->w * quat->x;
    f32_t wy = quat->w * quat->y;
    f32_t wz = quat->w * quat->z;
    f32_t xx = quat->x * quat->x;
    f32_t xy = quat->x * quat->y;
    f32_t xz = quat->x * quat->z;
    f32_t yy = quat->y * quat->y;
    f32_t yz = quat->y * quat->z;
    f32_t zz = quat->z * quat->z;

    result.m[0][0] = 2.0f * (ww + xx) - 1.0f;
    result.m[0][1] = 2.0f * (xy + wz);
    result.m[0][2] = 2.0f * (xz - wy);
    result.m[0][3] = 0.0f;
    result.m[1][0] = 2.0f * (xy - wz);
    result.m[1][1] = 2.0f * (ww + yy) - 1.0f;
    result.m[1][2] = 2.0f * (wx + yz);
    result.m[1][3] = 0.0f;
    result.m[2][0] = 2.0f * (wy + xz);
    result.m[2][1] = 2.0f * (yz - wx);
    result.m[2][2] = 2.0f * (ww + zz) - 1.0f;
    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

internal mat4_t
mat4_translation(vec3_t* translation)
{
    mat4_t result = {
        .m = {
            { 1.0f, 0.0f, 0.0f, translation->x },
            { 0.0f, 1.0f, 0.0f, translation->y },
            { 0.0f, 0.0f, 1.0f, translation->z },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };

    return result;
}

internal mat4_t
mat4_rotation(vec3_t* axis, f32_t angle)
{
    f32_t angle_cos   = math_cos(angle);
    f32_t angle_sin   = math_sin(angle);
    f32_t angle_sin_x = axis->x * angle_sin;
    f32_t angle_sin_y = axis->y * angle_sin;
    f32_t angle_sin_z = axis->z * angle_sin;
    f32_t one_sub_cos = 1.0f - angle_cos;

    mat4_t result;

    result.m[0][0] = axis->x * axis->x * one_sub_cos + angle_cos;
    result.m[0][1] = axis->x * axis->y * one_sub_cos + angle_sin_z;
    result.m[0][2] = axis->x * axis->z * one_sub_cos - angle_sin_y;
    result.m[0][3] = 0.0f;

    result.m[1][0] = axis->x * axis->y * one_sub_cos - angle_sin_z;
    result.m[1][1] = axis->y * axis->y * one_sub_cos + angle_cos;
    result.m[1][2] = axis->y * axis->z * one_sub_cos + angle_sin_x;
    result.m[1][3] = 0.0f;

    result.m[2][0] = axis->x * axis->z * one_sub_cos + angle_sin_y;
    result.m[2][1] = axis->y * axis->z * one_sub_cos - angle_sin_x;
    result.m[2][2] = axis->z * axis->z * one_sub_cos + angle_cos;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

internal mat4_t
mat4_scale(vec3_t* scale)
{
    mat4_t result = {
        .m = {
            { scale->x, 0.0f, 0.0f, 0.0f },
            { 0.0f, scale->y, 0.0f, 0.0f },
            { 0.0f, 0.0f, scale->z, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        }
    };

    return result;
}

internal mat4_t
mat4_model(vec3_t* position, quat_t* rotation, vec3_t* scale)
{
    mat4_t rot_matrix = mat4_from_quaternion(rotation);
    mat4_t scale_matrix = mat4_scale(scale);

    mat4_t result = mat4_translation(position);
    result        = mat4_mul(&result, &rot_matrix);
    result        = mat4_mul(&result, &scale_matrix);

    return result;
}

internal mat4_t
mat4_perspective(f32_t fov_x, f32_t aspect_ratio, f32_t clip_near, f32_t clip_far)
{
    f32_t inv_half_fov_tan = 1.0f / math_tan(fov_x * 0.5f);
    f32_t inv_aspect_ratio = 1.0f / aspect_ratio;
    f32_t far_minus_near   = clip_far - clip_near;

    mat4_t result = {};

    result.m[0][0] = inv_half_fov_tan * inv_aspect_ratio;
    result.m[1][1] = inv_half_fov_tan;
    result.m[2][2] = (clip_far + clip_near) / far_minus_near;
    result.m[2][3] = 1.0f;
    result.m[3][2] = -2.0f * clip_far * clip_near / far_minus_near;

    return result;
}

internal mat4_t
mat4_orthographic(f32_t clip_left, f32_t clip_right, f32_t clip_bottom, f32_t clip_top, f32_t clip_near, f32_t clip_far)
{
    mat4_t result = {};

    f32_t inv_right_minus_left = 1.0f / (clip_right - clip_left);
    f32_t inv_top_minus_bottom = 1.0f / (clip_top - clip_bottom);
    f32_t inv_far_minus_near   = 1.0f / (clip_far - clip_near);

    result.m[0][0] = 2.0f * inv_right_minus_left;
    result.m[1][1] = 2.0f * inv_top_minus_bottom;
    result.m[2][2] = 2.0f * inv_far_minus_near;
    result.m[3][0] = -(clip_right + clip_left) * inv_right_minus_left;
    result.m[3][1] = -(clip_top + clip_bottom) * inv_top_minus_bottom;
    result.m[3][2] = -(clip_far + clip_near) * inv_far_minus_near;
    result.m[3][3] = 1.0f;

    return result;
}

internal mat4_t
mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up)
{
    vec3_t eye_to_target = vec3_sub(target, eye);
    vec3_t forward       = vec3_normalize(&eye_to_target);
    vec3_t up_cross_fw   = vec3_cross(up, &forward);
    vec3_t right         = vec3_normalize(&up_cross_fw);
    vec3_t fw_cross_r    = vec3_cross(&forward, &right);
    vec3_t localUp       = vec3_normalize(&fw_cross_r);

    mat4_t result;

    result.m[0][0] = -right.x;
    result.m[0][1] =  localUp.x;
    result.m[0][2] =  forward.x;
    result.m[0][3] =  0.0f;

    result.m[1][0] = -right.y;
    result.m[1][1] =  localUp.y;
    result.m[1][2] =  forward.y;
    result.m[1][3] =  0.0f;

    result.m[2][0] = -right.z;
    result.m[2][1] =  localUp.z;
    result.m[2][2] =  forward.z;
    result.m[2][3] =  0.0f;

    result.m[3][0] =  vec3_dot(&right, eye);
    result.m[3][1] = -vec3_dot(&localUp, eye);
    result.m[3][2] = -vec3_dot(&forward, eye);
    result.m[3][3] =  1.0f;

    return result;
}

