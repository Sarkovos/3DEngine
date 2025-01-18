#include <math.h>
#include "matrix.h"


mat3_t mat3_identity(void)
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

mat3_t mat3_from_mat4(mat4_t m4) 
{
    mat3_t m3;

    // Copy the upper-left 3x3 elements
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            m3.m[i][j] = m4.m[i][j];
        }
    }
    return m3;
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

vec3_t mat3_mul_vec3(mat3_t m, vec3_t v) {
    vec3_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z;
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

mat3_t mat3_transpose(mat3_t m) {
    mat3_t result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

// Function to compute the inverse of a 3x3 matrix using Gauss-Jordan elimination
mat3_t mat3_inverse(mat3_t m) {
    mat3_t inverse;
    float temp;

    // Create an augmented matrix [m | I]
    float aug[3][3 * 2];

    // Initialize the augmented matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // Copy the original matrix
            aug[i][j] = m.m[i][j];
            // Initialize the identity matrix
            aug[i][j + 3] = (i == j) ? 1.0f : 0.0f;
        }
    }

    // Perform Gauss-Jordan elimination
    for (int i = 0; i < 3; i++) {
        // Partial pivoting
        float maxElement = fabs(aug[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < 3; k++) {
            if (fabs(aug[k][i]) > maxElement) {
                maxElement = fabs(aug[k][i]);
                maxRow = k;
            }
        }

        // Swap maximum row with current row (if needed)
        if (maxRow != i) {
            for (int k = 0; k < 3 * 2; k++) {
                float tmp = aug[i][k];
                aug[i][k] = aug[maxRow][k];
                aug[maxRow][k] = tmp;
            }
        }

        // Check for singularity
        if (fabs(aug[i][i]) < EPSILON) {
            fprintf(stderr, "Error: Singular matrix cannot be inverted.\n");
        }

        // Normalize the pivot row
        temp = aug[i][i];
        for (int j = 0; j < 3 * 2; j++) {
            aug[i][j] /= temp;
        }

        // Eliminate the other rows
        for (int k = 0; k < 3; k++) {
            if (k != i) {
                temp = aug[k][i];
                for (int j = 0; j < 3 * 2; j++) {
                    aug[k][j] -= aug[i][j] * temp;
                }
            }
        }
    }

    // Extract the inverse matrix from the augmented matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            inverse.m[i][j] = aug[i][j + 3];
        }
    }

    return inverse;
}

mat4_t mat4_look_at (vec3_t eye, vec3_t target, vec3_t up) 
{
    // Computer the forward (z), right (x), and up (y) vectors
    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z);
    vec3_t x = vec3_cross(up, z);
    vec3_normalize(&x);
    vec3_t y = vec3_cross(z, x);

    mat4_t view_matrix = {{
        {x.x, x.y, x.z, -vec3_dot(x, eye) },
        {y.x, y.y, y.z, -vec3_dot(y, eye) },
        {z.x, z.y, z.z, -vec3_dot(z, eye) },
        {  0,   0,   0,                 1 }
    }};
    return view_matrix;

}