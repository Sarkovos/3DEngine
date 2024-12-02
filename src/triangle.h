#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"
#include "texture.h"

#include <math.h>

// This stores the vertex index of our triangle
typedef struct {
    int a;
    int b;
    int c;
    int normal_a;
    int normal_b;
    int normal_c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t;

typedef struct {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

// This stores the actual vec2 points of the triangle on the screen
typedef struct {
    vec4_t points[3];
    tex2_t texcoords[3];
    color_t vertex_colors[3];
    vec3_t vertex_normals[3];
    color_t color;
} triangle_t;

void triangle_fill(triangle_t t, color_t color);
void triangle_fill_barycentric(triangle_t t, color_t vertexColors[3]);
bool backface_cull_check (triangle_t t);
bool is_top_left(vec2_t* start, vec2_t* end);
void draw_textured_triangle(color_t vertexColors[3], triangle_t triangle, uint32_t* texture);
uint32_t color_t_to_uint32(color_t color);
color_t uint32_to_color_t(uint32_t color);


#endif