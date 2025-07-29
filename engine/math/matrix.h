#ifndef MATRIX_H
#define MATRIX_H

internal f32_t mat4_determinant(mat4_t* matrix);

internal mat4_t mat4_add(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_sub(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_mul(mat4_t* lhs, mat4_t* rhs);
internal mat4_t mat4_mul_s(mat4_t* lhs, f32_t rhs);

internal vec3_t mat4_to_euler(mat4_t* matrix);
internal mat4_t mat4_from_euler(vec3_t* angles);
internal mat4_t mat4_from_pitch(f32_t pitch);
internal mat4_t mat4_from_yaw(f32_t yaw);
internal mat4_t mat4_from_roll(f32_t roll);
internal mat4_t mat4_from_diagonal(f32_t diagonal);
internal mat4_t mat4_from_quaternion(quat_t* quat);

internal mat4_t mat4_translation(vec3_t* translation);
internal mat4_t mat4_rotation(vec3_t* axis, f32_t angle);
internal mat4_t mat4_scale(vec3_t* scale);

internal mat4_t mat4_model(vec3_t* position, quat_t* rotation, vec3_t* scale);
internal mat4_t mat4_perspective(f32_t fov_x, f32_t aspect_ratio, f32_t near, f32_t far);
internal mat4_t mat4_orthographic(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far);
internal mat4_t mat4_look_at(vec3_t* eye, vec3_t* target, vec3_t* up);

#endif // MATRIX_H
