#ifndef MATH_CORE_H
#define MATH_CORE_H

#define MATH_PI            3.14159265358f
#define MATH_PI_SQR        9.86960506439f
#define MATH_PI_CUBE       31.0062789916f
#define MATH_PI_INV        0.31830987334f
#define MATH_PI_SQR_INV    0.10132117569f
#define MATH_PI_CUBE_INV   0.03225153312f
#define MATH_TWO_PI        6.28318530717f
#define MATH_HALF_PI       1.57079632679f
#define MATH_THREE_PI_HALF 4.71238899231f
#define MATH_RAD2DEG       57.2957763671f
#define MATH_DEG2RAD       0.01745329238f

#define MATH_EPSILON       1.192092896e-07f

#define MATH_FLT_MIN       1.175494351e-38f
#define MATH_FLT_MAX       3.402823466e+38f

#define POW_2_CHECK(v)         ((v)!=0 && ((v) & ((v) - 1))==0)
#define POW_2_ROUND_UP(v, a)   (((v) + (a) - 1) & (~((a) - 1)))
#define POW_2_ROUND_DOWN(v, a) ((v) & (~((a) - 1)))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define CLAMP_TOP(a, b)    MIN(a, b)
#define CLAMP_BOT(a, b)    MAX(a, b)
#define CLAMP(v, min, max) (((v) < (min)) ? (min) : ((v) > (max) ? (max) : (v)))

///////////////////////////
// FUNCTIONS FROM math.h

#define math_sin(v)      sinf(v)
#define math_cos(v)      cosf(v)
#define math_tan(v)      tanf(v)
#define math_asin(v)     asinf(v)
#define math_acos(v)     acosf(v)
#define math_atan(v)     atanf(v)
#define math_atan2(x, y) atan2f((x), (y))
#define math_mod(x, y)   fmodf((x), (y))
#define math_abs(v)      fabsf(v)
#define math_pow(v, e)   powf((v), (e))
#define math_sqrt(v)     sqrtf(v)
#define math_ceil(v)     ceilf(v)
#define math_floor(v)    floorf(v)

internal b32_t math_approx_zero(f32_t value);
internal b32_t math_approx_equal(f32_t a, f32_t b, f32_t error);

internal f32_t math_sign(f32_t value);
internal f32_t math_angle_wrap(f32_t angle);
internal f32_t math_saturate(f32_t value);
internal f32_t math_fast_sin(f32_t angle);
internal f32_t math_fast_cos(f32_t angle);
internal f32_t math_fast_cos_zero_to_half_pi(f32_t angle);
internal f32_t math_fast_inv_sqrt_approx_one(f32_t value);

#endif // MATH_CORE_H
