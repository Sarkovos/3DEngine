#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"
#include "triangle.h"

typedef struct {
    vec3_t direction;
} light_t;

extern light_t light;

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor);
color_t light_apply_intensity_color(color_t base_color, float light_intensity);

#endif