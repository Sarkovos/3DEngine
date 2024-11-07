#include "triangle.h"


float edge_cross(vec2_t* a, vec2_t* b, vec2_t* p)
{
    vec2_t ab = { b->x - a->x, b->y - a->y };
    vec2_t ap = { p->x - a->x, p->y - a->y };
    
    return ab.x * ap.y - ab.y * ap.x;

}
void triangle_fill(vec2_t v0, vec2_t v1, vec2_t v2, uint32_t color)
{
    // Find the bounding box with all canidate pixels
    int x_min = floor(fmin(fmin(v0.x, v1.x), v2.x));
    int y_min = floor(fmin(fmin(v0.y, v1.y), v2.y));
    int x_max = ceil(fmax(fmax(v0.x, v1.x), v2.x));
    int y_max = ceil(fmax(fmax(v0.y, v1.y), v2.y));

    float bias0 = is_top_left(&v1, &v2) ? 0 : -0.0001;
    float bias1 = is_top_left(&v2, &v0) ? 0 : -0.0001;
    float bias2 = is_top_left(&v0, &v1) ? 0 : -0.0001;

    // loop all canidate pixels inside the bounding box
    for (int y = y_min; y <= y_max; y++)
    {
        for (int x = x_min; x <= x_max; x++)
        {
            vec2_t p = {x, y};

            // check if the edges and the point all have a positive cross product
            // if all cross products are positive, this point is within the triangle
            float w0 = edge_cross(&v1, &v2, &p) + bias0;
            float w1 = edge_cross(&v2, &v0, &p) + bias1;
            float w2 = edge_cross(&v0, &v1, &p) + bias2;

            bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;
            if (is_inside)
            {

                draw_pixel(x, y, color);
            }
            
        }
    }
}

void triangle_fill_barycentric(vec2_t v0, vec2_t v1, vec2_t v2, color_t vertexColors[3])
{
    // Find the bounding box with all canidate pixels
    int x_min = floor(fmin(fmin(v0.x, v1.x), v2.x));
    int y_min = floor(fmin(fmin(v0.y, v1.y), v2.y));
    int x_max = ceil(fmax(fmax(v0.x, v1.x), v2.x));
    int y_max = ceil(fmax(fmax(v0.y, v1.y), v2.y));

    float bias0 = is_top_left(&v1, &v2) ? 0 : -0.0001;
    float bias1 = is_top_left(&v2, &v0) ? 0 : -0.0001;
    float bias2 = is_top_left(&v0, &v1) ? 0 : -0.0001;

    // Find the area of the entire triangle/parallelogram
    float area = edge_cross(&v0, &v1, &v2);



    // loop all canidate pixels inside the bounding box
    for (int y = y_min; y <= y_max; y++)
    {
        for (int x = x_min; x <= x_max; x++)
        {
            vec2_t p = {x, y};

            // check if the edges and the point all have a positive cross product
            // if all cross products are positive, this point is within the triangle
            float w0 = edge_cross(&v1, &v2, &p) + bias0;
            float w1 = edge_cross(&v2, &v0, &p) + bias1;
            float w2 = edge_cross(&v0, &v1, &p) + bias2;

            bool is_inside = w0 >= 0 && w1 >= 0 && w2 >= 0;
            if (is_inside)
            {

                // compute barycentric weights alpha, beta, and gamma
                float alpha = w0 / (float)area;
                float beta = w1 / (float)area;
                float gamma = w2 / (float)area;

                int a = 0xFF;
                int r = (alpha) * vertexColors[0].r + (beta) * vertexColors[1].r + (gamma) * vertexColors[2].r;
                int g = (alpha) * vertexColors[0].g + (beta) * vertexColors[1].g + (gamma) * vertexColors[2].g;
                int b = (alpha) * vertexColors[0].b + (beta) * vertexColors[1].b + (gamma) * vertexColors[2].b;

                // Combine the interpolated values into a uint32_t color
                uint32_t interp_color = 0x00000000;
                interp_color |= (a << 24);  // Alpha in the highest 8 bits
                interp_color |= (r << 16);  // Red in the next 8 bits
                interp_color |= (g << 8);   // Green in the next 8 bits
                interp_color |= b;          // Blue in the lowest 8 bits

                draw_pixel(x, y, interp_color);
            }
            
        }
    }
}

bool is_top_left(vec2_t* start, vec2_t* end)
{
    vec2_t edge = { end->x - start->x, end->y - start->y };

    bool is_top_edge = edge.y == 0 && edge.x < 0;
    bool is_left_edge = edge.y > 0;

    return is_top_edge || is_left_edge;

}

// in a left handed, counterclockwise, cross product sign is positive
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

    
    return (cross > 0);
} 