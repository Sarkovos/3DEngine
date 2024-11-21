#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"
#include "triangle.h"

typedef struct {
    vec3_t direction;
} light_t;

extern light_t light;

color_t light_apply_intensity(color_t original_color, float percentage_factor);

#endif