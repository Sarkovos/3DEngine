#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
#include <stdio.h>

typedef struct {
    float m[4][4];

} mat4_t;

typedef struct
{
	float m[3][3];
} mat3_t;

mat4_t mat4_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
mat4_t mat4_make_perspective_GL(float t, float b, float l, float r, float znear, float zfar);
mat4_t mat4_make_orthographic(float t, float b, float l, float r, float znear, float zfar);
vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v);
mat4_t mat4_transpose(mat4_t m);
mat4_t inverse4x4(mat4_t m);
mat4_t adjugate4x4(mat4_t mat);
float cofactor(mat4_t mat, int row, int col);
float determinant3x3(mat3_t mat);

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);

#endif