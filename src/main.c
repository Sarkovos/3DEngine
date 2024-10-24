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


enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum render_method {
    RENDER_WIRE,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE
} render_method;

enum projection_method {
    VERTICAL_PERSPECTIVE,
    HORIZONTAL_PERSPECTIVE,
    ORTHOGRAPHIC
} projection_method;


// OBJ file variable
char *filename = "./static/Amongus.obj"; 


/*Setup Functions to initialize variables and game objects*/
void setup(void) 
{
    //Initialze render mode and triangle culling method
    render_method = RENDER_FILL_TRIANGLE_WIRE;
    //cull_method = CULL_BACKFACE;
    projection_method = VERTICAL_PERSPECTIVE;

    //Allocate the required memory in bytes to hold the frame buffer
    frame_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height); //The cast here is not required, helps with porting between C and C++

    //creating an SDL texture that is used to display the frame buffer
    frame_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    // load mesh data
    //load_cube_mesh_data();
    //load_obj_file_data("./static/cube.obj");
    max_num_triangles_to_render = initialize_obj_file_data(filename);

    triangles_to_render = malloc(sizeof(triangle_t) * max_num_triangles_to_render);

    load_obj_file_data(filename);


    //Initialize the perspective projection matrix
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

    ortho_height= 10.0; 
    ortho_width = ortho_height * aspect;

    //Initialize projection matrix
    vertical_proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);
    horizontal_proj_matrix = mat4_make_perspective_GL(t, b, l, r, znear, zfar);
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
            if (event.key.keysym.sym == SDLK_1)
            {
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            }

            if (event.key.keysym.sym == SDLK_2)
            {
                render_method = RENDER_FILL_TRIANGLE;
            }

            if (event.key.keysym.sym == SDLK_3)
            {
                render_method = RENDER_WIRE;
            }

            if (event.key.keysym.sym == SDLK_9)
            {
                projection_method = HORIZONTAL_PERSPECTIVE;
            }

            if (event.key.keysym.sym == SDLK_8)
            {
                projection_method = VERTICAL_PERSPECTIVE;
            }

            if (event.key.keysym.sym == SDLK_7)
            {
                l = -ortho_width / 2.0f;
                r = ortho_width / 2.0f;
                b = -ortho_height / 2.0f;
                t = ortho_height / 2.0f;
                orthographic_matrix = mat4_make_orthographic(t, b, l, r, znear, zfar);
                projection_method = ORTHOGRAPHIC;
            }

            if (event.key.keysym.sym == SDLK_UP)
            {
                fov_degrees += 10.0;
                fov = fov_degrees * (PI / 180);
                r = znear * tan(fov / 2.0);
                l = -r;
                t = r / aspect;
                b = -t;

                vertical_proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);
                horizontal_proj_matrix = mat4_make_perspective_GL(t, b, l, r, znear, zfar);
            }

            if (event.key.keysym.sym == SDLK_DOWN)
            {
                fov_degrees -= 10.0;
                fov = fov_degrees * (PI / 180);
                r = znear * tan(fov / 2.0);
                l = -r;
                t = r / aspect;
                b = -t;

                vertical_proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);
                horizontal_proj_matrix = mat4_make_perspective_GL(t, b, l, r, znear, zfar);
            }
    }
}

void update(void)
{
    //wait some time until we reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    //only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    // initialize the counter of triangles to render for current frame
    num_triangles_to_render = 0;

    //mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    //mesh.rotation.z += 0.01;

    //mesh.scale.x += 0.002;
    //mesh.scale.y += 0.01;
    mesh.translation.z = 5.0;
    //mesh.translation.x += 0.01;

    //Create a scale matrix that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

    //Create a translate matrix that will be used to multiply the mesh vertices
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    //Create rotation matrices that will be used to multiply the mesh vertices
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    //loop all triangle faces of our mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t  mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // create a world matrix combining scale, rotation, and translation matrices
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);

            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);

            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            //save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        vec4_t projected_points[3];
        //loop all three vertices to perfrom projection
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

        triangle_t projected_triangle = {
            .points = {
                {projected_points[0].x, projected_points[0].y },
                {projected_points[1].x, projected_points[1].y },
                {projected_points[2].x, projected_points[2].y }
            }};

        //Save the projected triangle in the array of triangles to render
        //These are the triangles that are sent to the render function
        if (num_triangles_to_render < max_num_triangles_to_render)
        {
            triangles_to_render[num_triangles_to_render] = projected_triangle;
            num_triangles_to_render++;
        }
        

    }
}

void render(void)
{

    for (int i = 0; i < num_triangles_to_render; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        // COLORS FOR TESTING BARYCENTRIC COORDS
        triangle.point_colors[0] = (color_t){ .a = 0xFF, .r = 0xFF, .g = 0x00, .b = 0x00 };  // Red
        triangle.point_colors[1] = (color_t){ .a = 0xFF, .r = 0x00, .g = 0xFF, .b = 0x00 };  // Green
        triangle.point_colors[2] = (color_t){ .a = 0xFF, .r = 0x00, .g = 0x00, .b = 0xFF };  // Blue

        // Back face culling check, renders if it FAILS, doesnt render if it passes
        if (backface_cull_check(triangle.points[0], triangle.points[1], triangle.points[2]))
        {
            if (render_method == RENDER_FILL_TRIANGLE)
            {
                triangle_fill(triangle.points[0], triangle.points[1], triangle.points[2], triangle.point_colors);
            }

            if (render_method == RENDER_WIRE)
            {
                draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFFFFFF
                );
            }

            if (render_method == RENDER_FILL_TRIANGLE_WIRE)
            {
                triangle_fill(triangle.points[0], triangle.points[1], triangle.points[2], triangle.point_colors);
                draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFFFFFF
                );
            }
        }
            
    }

    render_frame_buffer();
    clear_frame_buffer(0xFF000000);

    SDL_RenderPresent(renderer);

}

// free the memory that was dynamically allocated by the program
void free_resources(void)
{
    array_free(mesh.faces);
    array_free(mesh.vertices);
    free(frame_buffer);
    free(triangles_to_render);
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