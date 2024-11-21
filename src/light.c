#include "light.h"

light_t light = {
    .direction = { 0, 0, 1 }
};

color_t light_apply_intensity(color_t original_color, float percentage_factor)
{

    color_t new_color = original_color;

    // Apply intensity to RGB components
    new_color.r = (uint8_t)(original_color.r * percentage_factor);
    new_color.g = (uint8_t)(original_color.g * percentage_factor);
    new_color.b = (uint8_t)(original_color.b * percentage_factor);

    // Set alpha component
    new_color.a = original_color.a;

	

	return new_color;
}

