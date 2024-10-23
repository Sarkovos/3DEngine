#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"

#include <math.h>

// This stores the vertex index of our triangle
typedef struct {
    int a;
    int b;
    int c;
} face_t;

typedef struct {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

// This stores the actual vec2 points of the triangle on the screen
typedef struct {
    vec2_t points[3];
    color_t point_colors[3];
} triangle_t;

void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2, color_t vertexColors[3]);
bool backface_cull_check (vec2_t v0, vec2_t v1, vec2_t v2);
bool is_top_left(vec2_t* start, vec2_t* end);


#endif