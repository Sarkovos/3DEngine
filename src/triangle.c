#include "triangle.h"


int edge_cross(vec2_t* a, vec2_t* b, vec2_t* p)
{
    vec2_t ab = { b->x - a->x, b->y - a->y };
    vec2_t ap = { p->x - a->x, p->y - a->y };
    return ab.x * ap.y - ab.y * ap.x;

}
void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2, uint32_t color)
{
    // Find the bounding box with all canidate pixels
    int x_min = fmin(fmin(v0.x, v1.x), v2.x);
    int y_min = fmin(fmin(v0.y, v1.y), v2.y);
    int x_max = fmax(fmax(v0.x, v1.x), v2.x);
    int y_max = fmax(fmax(v0.y, v1.y), v2.y);

    // loop all canidate pixels inside the bounding box
    for (int y = y_min; y <= y_max; y++)
    {
        for (int x = x_min; x <= x_max; x++)
        {
            vec2_t p = {x, y};

            int w0 = edge_cross(&v1, &v2, &p);
            int w1 = edge_cross(&v2, &v0, &p);
            int w2 = edge_cross(&v0, &v1, &p);

            bool is_inside = w0 <= 0 && w1 <= 0 && w2 <= 0;
            if (is_inside)
            {
                draw_pixel(x, y, color);
            }
            
        }
    }
}

// in a left handed, clockwise, y increase downward system, cross product sign is negative
bool backface_cull_check (vec2_t v0, vec2_t v1, vec2_t v2)
{
    vec2_t edge1;
    vec2_t edge2;

    // computer edge vectors
    edge1.x = v1.x - v0.x;
    edge1.y = v1.y - v0.y;
    edge2.x = v2.x - v0.x;
    edge2.y = v2.y - v0.y;

    // Compute the cross product
    float cross = edge1.x * edge2.y - edge1.y * edge2.x;

    
    return (cross < 0);
} 