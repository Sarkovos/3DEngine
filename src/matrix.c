#include <math.h>
#include "matrix.h"


mat3_t mat3x3_identity(void)
{
	mat3_t m =
	{ {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	} };

	return m;
}

mat4_t mat4_identity(void) {
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |
    mat4_t m = {{
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    }};
    return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    // | sx  0  0  0 |
    // |  0 sy  0  0 |
    // |  0  0 sz  0 |
    // |  0  0  0  1 |
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    // | 1  0  0  tx |
    // | 0  1  0  ty |
    // | 0  0  1  tz |
    // | 0  0  0  1  |
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

mat4_t mat4_make_rotation_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    // | 1  0  0  0 |
    // | 0  c -s  0 |
    // | 0  s  c  0 |
    // | 0  0  0  1 |
    mat4_t m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

mat4_t mat4_make_rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    // |  c  0  s  0 |
    // |  0  1  0  0 |
    // | -s  0  c  0 |
    // |  0  0  0  1 |
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}

mat4_t mat4_make_rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    // | c -s  0  0 |
    // | s  c  0  0 |
    // | 0  0  1  0 |
    // | 0  0  0  1 |
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b)
{
    mat4_t m;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }

    return m;
}

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
    /*
    // | (1/tan(fov/2))/aspect             0               0                   0
    // |                  0  2/tan(fov/2)               0                   0                 
    // |                  0             0      zf/(fz-zn) (-zf * fn)/(zf - zn)
    // |                  0             0               1                   0
      */

     mat4_t m = {{{ 0 }}};
     m.m[0][0] = (1 / tan(fov / 2)) / aspect;
     m.m[1][1] = 1 / tan(fov / 2);
     m.m[2][2] = zfar / (zfar - znear);
     m.m[2][3] = (-zfar * znear) / (zfar - znear);
     m.m[3][2] = 1.0;
     return m;

}

mat4_t mat4_make_perspective_GL(float t, float b, float l, float r, float znear, float zfar)
{
    /*
    // |             2n/r-l             0         r+l/r-l                   0
    // |                  0        2n/t-b         t+b/t-b                   0                 
    // |                  0             0        -f+n/f-n            -2fn/f-n
    // |                  0             0               1                   0
      */

     mat4_t m = {{{ 0 }}};
     m.m[0][0] = (2 * znear) / (r - l);
     m.m[0][2] = (r + l) / (r - l);
     m.m[1][1] = (2 * znear) / (t - b);
     m.m[1][2] = (t + b) / (t - b);
     m.m[2][2] = (-zfar + znear) / (zfar - znear);
     m.m[2][3] = (-zfar * znear) / (zfar - znear);
     m.m[3][2] = 1.0;
     return m;

}

mat4_t mat4_make_orthographic(float t, float b, float l, float r, float znear, float zfar)
{
    /*
    // |             2/r-l              0               0             -r+l/r-l 
    // |                  0         2/t-b               0            -t+b/t-b                  
    // |                  0             0          -2/f-n            -f+n/f-n
    // |                  0             0               0                   1
      */

     mat4_t m = {{{ 0 }}};
    m.m[0][0] = 2.0f / (r - l);
    m.m[1][1] = 2.0f / (t - b);
    m.m[2][2] = -2.0f / (zfar - znear);

    m.m[0][3] = -(r + l) / (r - l);
    m.m[1][3] = -(t + b) / (t - b);
    m.m[2][3] = -(zfar + znear) / (zfar - znear);
    m.m[3][3] = 1.0f;
     return m;

}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v)
{
    //multiply the projection matrix by our original vector
    vec4_t result = mat4_mul_vec4(mat_proj, v);

    //perform perspective divide with original z-value that is now stored in w
    if (result.w != 0.0)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    return result;
}

mat4_t mat4_transpose(mat4_t m) {
    mat4_t result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

// Function to calculate the inverse of a 4x4 matrix
mat4_t inverse4x4(mat4_t mat)
{
	mat4_t result = mat4_identity();

	// Calculate the determinant of the input matrix
	float det = mat.m[0][0] * cofactor(mat, 0, 0) +
		mat.m[0][1] * cofactor(mat, 0, 1) +
		mat.m[0][2] * cofactor(mat, 0, 2) +
		mat.m[0][3] * cofactor(mat, 0, 3);

	// Check if the determinant is zero (matrix is singular)
	if (det == 0.0f) {
		// Handle the error (e.g., print an error message or return an identity matrix)
		printf("Error: Singular matrix, inverse does not exist.\n");
		// Returning an identity matrix for simplicity; you might want to handle errors differently
		return result;
	}

	// Calculate the inverse by dividing the adjugate by the determinant
	float invDet = 1.0f / det;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = adjugate4x4(mat).m[i][j] * invDet;
		}
	}

	return result;
}

mat4_t adjugate4x4(mat4_t mat)
{
	mat4_t result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// Calculate the cofactor and transpose it to get the adjugate
			result.m[j][i] = cofactor(mat, i, j);
		}
	}

	return result;
}

float cofactor(mat4_t mat, int row, int col)
{
	int sign = ((row + col) & 2) == 0 ? 1 : -1;

	mat3_t mat3x3 =
	{ {
		{mat.m[(row + 1) % 4][(col + 1) % 4], mat.m[(row + 1) % 4][(col + 2) % 4], mat.m[(row + 1) % 4][(col + 3) % 4]},
		{mat.m[(row + 2) % 4][(col + 1) % 4], mat.m[(row + 2) % 4][(col + 2) % 4], mat.m[(row + 2) % 4][(col + 3) % 4]},
		{mat.m[(row + 3) % 4][(col + 1) % 4], mat.m[(row + 3) % 4][(col + 2) % 4], mat.m[(row + 3) % 4][(col + 3) % 4]}
	} };

	float determinant = determinant3x3(mat3x3);

	return sign * determinant;
}

// Function to calculate the determinant of a 3x3 matrix
float determinant3x3(mat3_t mat)
{
	return mat.m[0][0] * (mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1]) -
		mat.m[0][1] * (mat.m[1][0] * mat.m[2][2] - mat.m[1][2] * mat.m[2][0]) +
		mat.m[0][2] * (mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0]);
}


