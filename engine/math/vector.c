internal vec2_t
vec2_normalize(vec2_t* vector)
{
    f32_t inv_len = 1.0f / vec2_length(vector);

    vec2_t result = {
        vector->x * inv_len,
        vector->y * inv_len
    };

    return result;
}

internal vec3_t
vec3_normalize(vec3_t* vector)
{
    f32_t inv_len = 1.0f / vec3_length(vector);

    vec3_t result = {
        vector->x * inv_len,
        vector->y * inv_len,
        vector->z * inv_len
    };

    return result;
}

internal vec4_t
vec4_normalize(vec4_t* vector)
{
    f32_t inv_len = 1.0f / vec4_length(vector);

    vec4_t result = {
        vector->x * inv_len,
        vector->y * inv_len,
        vector->z * inv_len,
        vector->w * inv_len
    };

    return result;
}

internal vec2_t
vec2_negate(vec2_t* vector)
{
    vec2_t result = {
        -vector->x,
        -vector->y
    };

    return result;
}

internal vec3_t
vec3_negate(vec3_t* vector)
{
    vec3_t result = {
        -vector->x,
        -vector->y,
        -vector->z
    };

    return result;
}

internal vec4_t
vec4_negate(vec4_t* vector)
{
    vec4_t result = {
        -vector->x,
        -vector->y,
        -vector->z,
        -vector->w
    };

    return result;
}

internal vec2_t
vec2_add(vec2_t* lhs, vec2_t* rhs)
{
    vec2_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y
    };

    return result;
}

internal vec3_t
vec3_add(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z
    };

    return result;
}

internal vec4_t
vec4_add(vec4_t* lhs, vec4_t* rhs)
{
    vec4_t result = {
        lhs->x + rhs->x,
        lhs->y + rhs->y,
        lhs->z + rhs->z,
        lhs->w + rhs->w
    };

    return result;
}

internal vec2_t
vec2_sub(vec2_t* lhs, vec2_t* rhs)
{
    vec2_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y
    };

    return result;
}

internal vec3_t
vec3_sub(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z
    };

    return result;
}

internal vec4_t
vec4_sub(vec4_t* lhs, vec4_t* rhs)
{
    vec4_t result = {
        lhs->x - rhs->x,
        lhs->y - rhs->y,
        lhs->z - rhs->z,
        lhs->w - rhs->w
    };

    return result;
}

internal vec2_t
vec2_mul(vec2_t* lhs, vec2_t* rhs)
{
    vec2_t result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y
    };

    return result;
}

internal vec3_t
vec3_mul(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y,
        lhs->z * rhs->z
    };

    return result;
}

internal vec4_t
vec4_mul(vec4_t* lhs, vec4_t* rhs)
{
    vec4_t result = {
        lhs->x * rhs->x,
        lhs->y * rhs->y,
        lhs->z * rhs->z,
        lhs->w * rhs->w
    };

    return result;
}

internal vec2_t
vec2_mul_s(vec2_t* lhs, f32_t rhs)
{
    vec2_t result = {
        lhs->x * rhs,
        lhs->y * rhs
    };

    return result;
}

internal vec3_t
vec3_mul_s(vec3_t* lhs, f32_t rhs)
{
    vec3_t result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs
    };

    return result;
}

internal vec4_t
vec4_mul_s(vec4_t* lhs, f32_t rhs)
{
    vec4_t result = {
        lhs->x * rhs,
        lhs->y * rhs,
        lhs->z * rhs,
        lhs->w * rhs
    };

    return result;
}

internal f32_t
vec2_dot(vec2_t* lhs, vec2_t* rhs)
{
    f32_t result =
        lhs->x * rhs->x +
        lhs->y * rhs->y;

    return result;
}

internal f32_t
vec3_dot(vec3_t* lhs, vec3_t* rhs)
{
    f32_t result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z;

    return result;
}

internal f32_t
vec4_dot(vec4_t* lhs, vec4_t* rhs)
{
    f32_t result =
        lhs->x * rhs->x +
        lhs->y * rhs->y +
        lhs->z * rhs->z +
        lhs->w * rhs->w;

    return result;
}

internal f32_t
vec2_length(vec2_t* vector)
{
    f32_t x_sqr = vector->x * vector->x;
    f32_t y_sqr = vector->y * vector->y;

    return math_sqrt(x_sqr + y_sqr);
}

internal f32_t
vec3_length(vec3_t* vector)
{
    f32_t x_sqr = vector->x * vector->x;
    f32_t y_sqr = vector->y * vector->y;
    f32_t z_sqr = vector->z * vector->z;

    return math_sqrt(x_sqr + y_sqr + z_sqr);
}

internal f32_t
vec4_length(vec4_t* vector)
{
    f32_t x_sqr = vector->x * vector->x;
    f32_t y_sqr = vector->y * vector->y;
    f32_t z_sqr = vector->z * vector->z;
    f32_t w_sqr = vector->w * vector->w;

    return math_sqrt(x_sqr + y_sqr + z_sqr + w_sqr);
}

internal f32_t
vec2_length_sqr(vec2_t* vector)
{
    f32_t result =
        vector->x * vector->x +
        vector->y * vector->y;

    return result;
}

internal f32_t
vec3_length_sqr(vec3_t* vector)
{
    f32_t result =
        vector->x * vector->x +
        vector->y * vector->y +
        vector->z * vector->z;

    return result;
}

internal f32_t
vec4_length_sqr(vec4_t* vector)
{
    f32_t result =
        vector->x * vector->x +
        vector->y * vector->y +
        vector->z * vector->z +
        vector->w * vector->w;

    return result;
}

internal vec3_t
vec3_cross(vec3_t* lhs, vec3_t* rhs)
{
    vec3_t result = {
        lhs->y * rhs->z - lhs->z * rhs->y,
        lhs->z * rhs->x - lhs->x * rhs->z,
        lhs->x * rhs->y - lhs->y * rhs->x
    };

    return result;
}

// internal vec3_t
// vec3_rotate_quat(vec3_t* vector, quat_t* quat)
// {
//     return *vector;
// }
// 
// internal vec3_t
// vec3_rotate_axis(vec3_t* vector, vec3_t* axis, f32_t angle)
// {
//     return *vector;
// }

