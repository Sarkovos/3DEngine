#include "triangle.h"


float edge_cross(vec2_t* a, vec2_t* b, vec2_t* p)
{
    vec2_t ab = { b->x - a->x, b->y - a->y };
    vec2_t ap = { p->x - a->x, p->y - a->y };
    
    return ab.x * ap.y - ab.y * ap.x;

}
void triangle_fill(triangle_t t, color_t color)
{
    vec2_t v0 = { t.points[0].x, t.points[0].y };
    vec2_t v1 = { t.points[1].x, t.points[1].y };
    vec2_t v2 = { t.points[2].x, t.points[2].y };

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
                draw_pixel(x, y, color_t_to_uint32(color));
            }
            
        }
    }
}

void triangle_fill_barycentric(triangle_t t, color_t vertexColors[3])
{
    vec2_t v0 = { t.points[0].x, t.points[0].y };
    vec2_t v1 = { t.points[1].x, t.points[1].y };
    vec2_t v2 = { t.points[2].x, t.points[2].y };

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

                // // Clamp color values
                // r = r < 0 ? 0 : (r > 255 ? 255 : r);
                // g = g < 0 ? 0 : (g > 255 ? 255 : g);
                // b = b < 0 ? 0 : (b > 255 ? 255 : b);


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
bool backface_cull_check (triangle_t t)
{
    vec2_t edge1;
    vec2_t edge2;

    vec2_t v0 = { t.points[0].x, t.points[0].y };
    vec2_t v1 = { t.points[1].x, t.points[1].y };
    vec2_t v2 = { t.points[2].x, t.points[2].y };

    // computer edge vectors
    edge1.x = v1.x - v0.x;
    edge1.y = v1.y - v0.y;
    edge2.x = v2.x - v0.x;
    edge2.y = v2.y - v0.y;

    // Compute the cross product
    float cross = edge1.x * edge2.y - edge1.y * edge2.x;

    
    return (cross > 0);
} 

uint32_t color_t_to_uint32(color_t c) {
    return ((uint32_t)c.a << 24) |
           ((uint32_t)c.r << 16) |
           ((uint32_t)c.g << 8) |
           (uint32_t)c.b;
}

color_t uint32_to_color_t(uint32_t color) {
    color_t c;
    c.a = (color >> 24) & 0xFF;
    c.r = (color >> 16) & 0xFF;
    c.g = (color >> 8) & 0xFF;
    c.b = color & 0xFF;
    return c;
}

void draw_textured_triangle(color_t vertexColors[3], triangle_t triangle, uint32_t* texture)
{
    vec2_t v0 = { triangle.points[0].x, triangle.points[0].y };
    vec2_t v1 = { triangle.points[1].x, triangle.points[1].y };
    vec2_t v2 = { triangle.points[2].x, triangle.points[2].y };

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

                // get our uv texture coordinates
                float u0 = triangle.texcoords[0].u;
                float v_0 = triangle.texcoords[0].v;

                float u1 = triangle.texcoords[1].u;
                float v_1 = triangle.texcoords[1].v;

                float u2 = triangle.texcoords[2].u;
                float v_2 = triangle.texcoords[2].v;

                // variables to store the interpolated values of U, V, and 1/w for the current pixel
                float interpolated_u;
                float interpolated_v;
                float interpolated_reciprocal_w;

                // perform the interpolation of all U/w and V/w values using barycentric weights and a factpr pf 1/w
                interpolated_u = alpha * (u0 / triangle.points[0].w) + (u1 / triangle.points[1].w) * beta + (u2 / triangle.points[2].w) * gamma;
                interpolated_v = alpha * (v_0 / triangle.points[0].w) + (v_1 / triangle.points[1].w) * beta + (v_2 / triangle.points[2].w) * gamma;

                // also interpolate the value of 1/w for the current pixel
                interpolated_reciprocal_w = (1 / triangle.points[0].w) * alpha + (1 / triangle.points[1].w) * beta + (1 / triangle.points[2].w) * gamma;

                // now we can divide back both interpolated values by 1/w
                interpolated_u /= interpolated_reciprocal_w;
                interpolated_v /= interpolated_reciprocal_w;

                int tex_x = abs((int)(interpolated_u * texture_width));
                int tex_y = abs((int)(interpolated_v * texture_height));


                draw_pixel(x, y, texture[(texture_width * tex_y)  + tex_x]);
            }
            
        }
    }
}




