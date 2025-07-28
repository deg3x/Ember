internal b32_t
math_approx_zero(f32_t value)
{
    return (value > -MATH_FLT_MIN) && (value < MATH_FLT_MIN);
}

internal b32_t
math_approx_equal(f32_t a, f32_t b, f32_t error)
{
    return ((a + error) > b) && ((a - error) < b);
}

internal f32_t
math_sign(f32_t value)
{
    if (value < 0.0f)
    {
        return -1.0f;
    }

    if (value > 0.0f)
    {
        return 1.0f;
    }

    return 0.0f;
}

internal f32_t
math_angle_wrap(f32_t angle)
{
    f32_t result = math_mod(angle, MATH_TWO_PI);

    if (result < 0.0f)
    {
        return result + MATH_TWO_PI;
    }

    return result;
}

internal f32_t
math_saturate(f32_t value)
{
    return CLAMP(value, 0.0f, 1.0f);
}

internal f32_t
math_fast_sin(f32_t angle)
{
    return math_fast_cos(angle - MATH_HALF_PI);
}

internal f32_t
math_fast_cos(f32_t angle)
{
    f32_t angle_wrapped = math_angle_wrap(angle);

    if (angle_wrapped < MATH_HALF_PI)
    {
        return math_fast_cos_zero_to_half_pi(angle_wrapped);
    }

    if (angle_wrapped < MATH_PI)
    {
        return -math_fast_cos_zero_to_half_pi(MATH_PI - angle_wrapped);
    }

    if (angle_wrapped < MATH_THREE_PI_HALF)
    {
        return -math_fast_cos_zero_to_half_pi(angle_wrapped - MATH_PI);
    }

    return math_fast_cos_zero_to_half_pi(MATH_TWO_PI - angle_wrapped);
}

internal f32_t
math_fast_cos_zero_to_half_pi(f32_t angle)
{
    f32_t a0 = 1.0f;
    f32_t a2 = 2.0f * MATH_PI_INV - 12.0f * MATH_PI_SQR_INV;
    f32_t a3 = 16.0f * MATH_PI_CUBE_INV - 4.0f * MATH_PI_SQR_INV;

    f32_t angle_sqr = angle * angle;

    return a0 + a2 * angle_sqr + a3 * angle_sqr * angle;
}

internal f32_t
math_fast_inv_sqrt_approx_one(f32_t value)
{
    f32_t a0 =  1.875f;
    f32_t a1 = -1.250f;
    f32_t a2 =  0.375f;

    return a0 + a1 * value + a2 * value * value;
}
