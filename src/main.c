#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdint.h>

//personal header files
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "array.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"

#include "upng.h"

/*Global variables*/
mat4_t vertical_proj_matrix;
mat4_t horizontal_proj_matrix;
mat4_t orthographic_matrix;
float fov_degrees = 60;
float r, l, t, b, znear, zfar, fov, aspect, ortho_height, ortho_width;

triangle_t* triangles_to_render;
int num_triangles_to_render = 0;
int max_num_triangles_to_render = 0;

bool is_running = false;
int previous_frame_time;

const float PI = 3.14159265358979;

/*Input variables*/
enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum z_method {
    Z_NONE,
    Z_RENDER
} z_method;

enum render_method {
    RENDER_WIRE = 0,
    RENDER_FILL_TRIANGLE = 1,
    RENDER_FILL_TRIANGLE_WIRE = 2,
    RENDER_TEXTURED = 3,
    RENDER_TEXTURED_WIRE = 4
} render_method;

enum projection_method {
    VERTICAL_PERSPECTIVE = 0,
    HORIZONTAL_PERSPECTIVE = 1,
    ORTHOGRAPHIC = 2
} projection_method;

enum lighting_method
{
    NO_LIGHTING = 0,
    FLAT_SHADING = 1,
    Z_SHADING = 2,
} lighting_method;

// OBJ file variable
char *filename = "./static/LP"; 

void init_perspective_matrix()
{
    // Initialize the perspective projection matrix
    aspect = (float)window_width / (float)window_height;

    // Field of view in radians
    fov = fov_degrees * (PI / 180);

    // Near and far plane distances
    znear = 0.1f;
    zfar = 100.0f;

    // Calculate t and b
    r = znear * tan(fov / 2.0);
    l = -r;

    // Calculate r and l
    t = r / aspect;
    b = -t;

    ortho_height= 5.0; 
    ortho_width = ortho_height * aspect;

    //Initialize projection matrix
    vertical_proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);
    horizontal_proj_matrix = mat4_make_perspective_GL(t, b, l, r, znear, zfar);
}

void init_orthographic_matrix()
{
    l = -ortho_width / 2.0f;
    r = ortho_width / 2.0f;
    b = -ortho_height / 2.0f;
    t = ortho_height / 2.0f;
    orthographic_matrix = mat4_make_orthographic(t, b, l, r, znear, zfar);
}

// Setup Functions to initialize variables and game objects
void setup(void) 
{
    // Initialze render mode and triangle culling method
    render_method = RENDER_FILL_TRIANGLE_WIRE;

    projection_method = VERTICAL_PERSPECTIVE;

    // Allocate the required memory in bytes to hold the frame buffer
    frame_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height); //The cast here is not required, helps with porting between C and C++

    // Allocate the required memory in bytes to hold the z buffer
    z_buffer = (float*) malloc(sizeof(float) * window_width * window_height);

    // Creating an SDL texture that is used to display the frame buffer
    frame_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    // Buffers to hold the full filenames
    char filename_obj[256];
    char filename_png[256];

    // Create filenames by appending extensions
    snprintf(filename_obj, sizeof(filename_obj), "%s.obj", filename);
    snprintf(filename_png, sizeof(filename_png), "%s.png", filename);

    // Load the texture information from an external PNG file
    load_png_texture_data(filename_png);

    // Load mesh data
    //load_cube_mesh_data();
    //max_num_triangles_to_render = 12;

    load_obj_file_data(filename_obj);
    max_num_triangles_to_render = initialize_obj_file_data(filename_obj);
    
    triangles_to_render = malloc(sizeof(triangle_t) * max_num_triangles_to_render);

    init_perspective_matrix();
    init_orthographic_matrix();   
}



/*Where you handle input*/
void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_QUIT:
            is_running = false;
            break;
            
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
                break;
            }

            if (event.key.keysym.sym == SDLK_r)
            {
                render_method = (enum render_method)((render_method + 1) % 5);
                break;               
            }


            if (event.key.keysym.sym == SDLK_p)
            {
                projection_method = (enum projection_method)((projection_method + 1) % 3);
                break; 
            }

            if (event.key.keysym.sym == SDLK_l)
            {
                lighting_method = (enum lighting_method)((lighting_method + 1) % 3);
                break;
            }

            if (event.key.keysym.sym == SDLK_UP)
            {
                fov_degrees += 10.0;
                init_perspective_matrix();
                break;
            }

            if (event.key.keysym.sym == SDLK_DOWN)
            {
                fov_degrees -= 10.0;
                init_perspective_matrix();
                break;
            }

            if (event.key.keysym.sym == SDLK_SPACE)
            {
                // update();
                // render();
            }
            break;
    }
}

void update(void)
{
    // Wait some time until we reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    // Initialize the counter of triangles to render for current frame
    num_triangles_to_render = 0;


    // Mesh Transformations
    //mesh.rotation.x += 0.01;
    mesh.rotation.y -= 0.01;
    //mesh.rotation.z += 0.01;

    //mesh.scale.x += 0.002;
    //mesh.scale.y += 0.01;
    mesh.translation.z = 5.0;
    //mesh.translation.x += 0.01;

    // Create a scale matrix that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    // Create a translation matrix that will be used to multiply the mesh vertices
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    // Create rotation matrices that will be used to multiply the mesh vertices
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    // Compute the World Matrix by combining scale, rotation, and translation matrices
    mat4_t world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    mat3_t normals_world_matrix = mat3_from_mat4(world_matrix);

    normals_world_matrix = mat3_inverse(normals_world_matrix);
    normals_world_matrix = mat3_transpose(normals_world_matrix);
    
    // Loop all triangle faces of our mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t  mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec3_t vertex_normals[3];
        vertex_normals[0] = mesh.normals[mesh_face.normal_a];
        vertex_normals[1] = mesh.normals[mesh_face.normal_b];
        vertex_normals[2] = mesh.normals[mesh_face.normal_c];

        color_t vertex_colors[3];
        vertex_colors[0] = (color_t){ .a = 0xFF, .r = 0xFF, .g = 0xFF, .b = 0xFF }; 
        vertex_colors[1] = (color_t){ .a = 0xFF, .r = 0x00, .g = 0xFF, .b = 0xFF }; 
        vertex_colors[2] = (color_t){ .a = 0xFF, .r = 0x00, .g = 0xFF, .b = 0xFF }; 

        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // MAJOR NOTE TO SELF: THIS IS JUST DOING CROSS PRODUCT STUFF AGAIN
        // WE SHOULD REALLY CHANGE IT BUT IF YOU SEE THIS ON GITHUB OR SOMETHING
        // KNOW THAT I AM A BIT DUMB
        // Get individual vectors from A, B, and C vertices to compute normal
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

        // Get the vector subtraction of B-A and C-A
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        // Compute the face normal (using cross product to find perpendicular)
        vec3_t face_normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&face_normal);

		vec3_t transformed_normals[3];

		// APPLY TRANSFORMATION TO NORMALS
		for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_normal = mat3_mul_vec3(normals_world_matrix, vertex_normals[j]);

			// Normalize the transformed normal
            vec3_normalize(&transformed_normal);

            // Save the transformed normal
            transformed_normals[j] = transformed_normal;
		}

        vec4_t projected_points[3];

        // Loop all three vertices to perfrom projection
        if (projection_method == VERTICAL_PERSPECTIVE)
        {
            for (int j = 0; j < 3; j++)
            {
                //project the current vertex
                projected_points[j] = mat4_mul_vec4_project(vertical_proj_matrix, transformed_vertices[j]);

                //translate the projected points to the middle of the screen
                projected_points[j].x = ((projected_points[j].x + 1.0f) / 2.0f) * window_width;
                projected_points[j].y = ((projected_points[j].y + 1.0f) / 2.0f) * window_height;
            }
        }

        if (projection_method == HORIZONTAL_PERSPECTIVE)
        {
            for (int j = 0; j < 3; j++)
            {
                //project the current vertex
                projected_points[j] = mat4_mul_vec4_project(horizontal_proj_matrix, transformed_vertices[j]);

                //translate the projected points to the middle of the screen
                projected_points[j].x = ((projected_points[j].x + 1.0f) / 2.0f) * window_width;
                projected_points[j].y = ((projected_points[j].y + 1.0f) / 2.0f) * window_height;
            }
        }

        if (projection_method == ORTHOGRAPHIC)
        {
            for (int j = 0; j < 3; j++)
            {
                //project the current vertex
                projected_points[j] = mat4_mul_vec4(orthographic_matrix, transformed_vertices[j]);

                //translate the projected points to the middle of the screen
                projected_points[j].x = ((projected_points[j].x + 1.0f) / 2.0f) * window_width;
                projected_points[j].y = ((projected_points[j].y + 1.0f) / 2.0f) * window_height;
            }
        }
        

        // Create the triangle
        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
            },
            .vertex_colors = {
                vertex_colors[0],
                vertex_colors[1],
                vertex_colors[2],
            },
            .vertex_normals = {
					transformed_normals[0],
					transformed_normals[1],
					transformed_normals[2]
            },
            .texcoords = {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            }
        };
        
        if (lighting_method == FLAT_SHADING)
        {
            projected_triangle.vertex_normals[0] = face_normal;
            projected_triangle.vertex_normals[1] = face_normal;
            projected_triangle.vertex_normals[2] = face_normal;
        }

        // Save the projected triangle in the array of triangles to render
        // These are the triangles that are sent to the render function
        if (num_triangles_to_render < max_num_triangles_to_render)
        {
            triangles_to_render[num_triangles_to_render] = projected_triangle;
            num_triangles_to_render++;
        }
        
    }
}

void triangle_render(triangle_t triangle)
{   if (projection_method == ORTHOGRAPHIC)
    {
        if (render_method == RENDER_FILL_TRIANGLE)
        {
            triangle_fill_orthographic(triangle, triangle.color);
        }

        if (render_method == RENDER_WIRE)
        {
            draw_triangle(triangle, 0xFFFFFFFF);    
        }

        if (render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            triangle_fill_orthographic(triangle, triangle.color);
            draw_triangle(triangle, 0xFFFF0000);       
        }

        if (render_method == RENDER_TEXTURED)
        {
            draw_textured_triangle_orthographic(triangle.vertex_colors, triangle, mesh_texture);
        }

        if (render_method == RENDER_TEXTURED_WIRE)
        {
            draw_textured_triangle_orthographic(triangle.vertex_colors, triangle, mesh_texture);
            draw_triangle(triangle, 0xFFFFFFFF);
        }
    }

    else
    {
        if (render_method == RENDER_FILL_TRIANGLE)
        {
            triangle_fill(triangle, triangle.color);
        }

        if (render_method == RENDER_WIRE)
        {
            draw_triangle(triangle, 0xFFFFFFFF);    
        }

        if (render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            triangle_fill(triangle, triangle.color);
            draw_triangle(triangle, 0xFFFF0000);       
        }

        if (render_method == RENDER_TEXTURED)
        {
            draw_textured_triangle(triangle.vertex_colors, triangle, mesh_texture);
        }

        if (render_method == RENDER_TEXTURED_WIRE)
        {
            draw_textured_triangle(triangle.vertex_colors, triangle, mesh_texture);
            draw_triangle(triangle, 0xFFFFFFFF);
        }
    }
}

void render(void)
{
    for (int i = 0; i < num_triangles_to_render; i++)
    {
        triangle_t triangle = triangles_to_render[i];
        color_t triangle_color = (color_t){ .a = 0xFF, .r = 0xFF, .g = 0xFF, .b = 0xFF }; 
     
        // TODO: FIX BACKFACE CULLING TO BE BEFORE PROJECTION
        if (backface_cull_check(triangle))
        {
            switch (lighting_method)
            {
                // This case uses just the basic triangle_fill function, which only cares about the face color
                case NO_LIGHTING:
                {
                    triangle.color = triangle_color;
                    
                    triangle_render(triangle);
                    break;
                }

                case FLAT_SHADING:
                {
                    // calculate the shade intensity based on how aligned is the face normal and the light ray
                    float light_intensity_factor = vec3_dot(triangle.vertex_normals[0], light.direction);

                    // calculate the triangle color based on the light angle
                    triangle_color = light_apply_intensity(triangle.vertex_colors[0], light_intensity_factor);
                    
                    triangle.color = triangle_color;

                    triangle_render(triangle);
                    break;
                }

                case Z_SHADING:
                {
                    triangle_z_buffer(triangle);
                }
            }
        }     
    }

    render_frame_buffer();
    clear_frame_buffer(0xFF000000);
    clear_z_buffer();

    SDL_RenderPresent(renderer);

}

// Free the memory that was dynamically allocated by the program
void free_resources(void)
{
    array_free(mesh.faces);
    array_free(mesh.vertices);
    array_free(mesh.normals);
    array_free(mesh.vertex_colors);
    free(frame_buffer);
    free(triangles_to_render);
    free(z_buffer);
    upng_free(png_texture);
}

int main(void){

    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}