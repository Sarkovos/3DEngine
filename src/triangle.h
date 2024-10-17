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

// This stores the actual vec2 points of the triangle on the screen
typedef struct {
    vec2_t points[3];
} triangle_t;

void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2, uint32_t color);
bool backface_cull_check (vec2_t v0, vec2_t v1, vec2_t v2);


#endif