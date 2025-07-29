#ifndef VECTOR_H
#define VECTOR_H

internal vec2_t vec2_normalize(vec2_t* vector);
internal vec3_t vec3_normalize(vec3_t* vector);
internal vec4_t vec4_normalize(vec4_t* vector);

internal vec2_t vec2_negate(vec2_t* vector);
internal vec3_t vec3_negate(vec3_t* vector);
internal vec4_t vec4_negate(vec4_t* vector);

internal vec2_t vec2_add(vec2_t* lhs, vec2_t* rhs);
internal vec3_t vec3_add(vec3_t* lhs, vec3_t* rhs);
internal vec4_t vec4_add(vec4_t* lhs, vec4_t* rhs);
internal vec2_t vec2_sub(vec2_t* lhs, vec2_t* rhs);
internal vec3_t vec3_sub(vec3_t* lhs, vec3_t* rhs);
internal vec4_t vec4_sub(vec4_t* lhs, vec4_t* rhs);
internal vec2_t vec2_mul(vec2_t* lhs, vec2_t* rhs);
internal vec3_t vec3_mul(vec3_t* lhs, vec3_t* rhs);
internal vec4_t vec4_mul(vec4_t* lhs, vec4_t* rhs);
internal vec2_t vec2_mul_s(vec2_t* lhs, f32_t rhs);
internal vec3_t vec3_mul_s(vec3_t* lhs, f32_t rhs);
internal vec4_t vec4_mul_s(vec4_t* lhs, f32_t rhs);

internal f32_t vec2_dot(vec2_t* lhs, vec2_t* rhs);
internal f32_t vec3_dot(vec3_t* lhs, vec3_t* rhs);
internal f32_t vec4_dot(vec4_t* lhs, vec4_t* rhs);

internal f32_t vec2_length(vec2_t* vector);
internal f32_t vec3_length(vec3_t* vector);
internal f32_t vec4_length(vec4_t* vector);

internal f32_t vec2_length_sqr(vec2_t* vector);
internal f32_t vec3_length_sqr(vec3_t* vector);
internal f32_t vec4_length_sqr(vec4_t* vector);

internal vec3_t vec3_cross(vec3_t* lhs, vec3_t* rhs);
// internal vec3_t vec3_rotate_quat(vec3_t* vector, quat_t* quat);
// internal vec3_t vec3_rotate_axis(vec3_t* vector, vec3_t* axis, f32_t angle);

#endif // VECTOR_H
