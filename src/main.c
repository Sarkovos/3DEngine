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

enum render_method {
    RENDER_WIRE,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE
} render_method;

int render_method_tracker = 0;

enum projection_method {
    VERTICAL_PERSPECTIVE,
    HORIZONTAL_PERSPECTIVE,
    ORTHOGRAPHIC
} projection_method;

int projection_method_tracker = 0;




// OBJ file variable
char *filename = "./static/Amongus.obj"; 

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

    ortho_height= 10.0; 
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

/*Setup Functions to initialize variables and game objects*/
void setup(void) 
{
    // Initialze render mode and triangle culling method
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
                render_method_tracker++;
                switch(render_method_tracker % 3)
                {
                    case 0:
                        render_method = RENDER_FILL_TRIANGLE_WIRE;
                        break;
                    case 1: 
                        render_method = RENDER_FILL_TRIANGLE;
                        break;
                    case 2:
                        render_method = RENDER_WIRE;
                        break;
                }
                
            }


            if (event.key.keysym.sym == SDLK_p)
            {
                projection_method_tracker++;
                switch(projection_method_tracker % 3)
                {
                    case 0:
                        projection_method = HORIZONTAL_PERSPECTIVE;
                        break;
                    case 1: 
                        projection_method = VERTICAL_PERSPECTIVE;
                        break;
                    case 2:
                        projection_method = ORTHOGRAPHIC;
                        break;
                }
                
            }

            if (event.key.keysym.sym == SDLK_UP)
            {
                fov_degrees += 10.0;
                init_perspective_matrix();
            }

            if (event.key.keysym.sym == SDLK_DOWN)
            {
                fov_degrees -= 10.0;
                init_perspective_matrix();
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

    // Compute the World Matrix by combining scale, rotation, and translation matrices
    mat4_t world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

     // **Compute the Normal Matrix (inverse transpose of the world matrix)**
    mat4_t normal_matrix = inverse4x4(world_matrix);     // Compute inverse of world matrix
    normal_matrix = mat4_transpose(normal_matrix);         // Transpose the inverse to get the normal matrix

    //loop all triangle faces of our mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++)
    {
        face_t  mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t face_normals[3];
        face_normals[0] = mesh.normals[mesh_face.normal_a - 1];
        face_normals[1] = mesh.normals[mesh_face.normal_b - 1];
        face_normals[2] = mesh.normals[mesh_face.normal_c - 1];

        vec4_t transformed_vertices[3];
        vec3_t transformed_normals[3];
        color_t vertex_colors[3];

        // loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            //save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;

            // transform the vertex normal
            vec4_t normal = { face_normals[j].x, face_normals[j].y, face_normals[j].z, 0.0f };
            normal = mat4_mul_vec4(normal_matrix, normal);
            transformed_normals[j] = vec3_from_vec4(normal);
            vec3_normalize(&transformed_normals[j]);

            vec3_normalize(&light.direction);

            // Compute lighting at the vertex
            float light_intensity = vec3_dot(transformed_normals[j], light.direction);
            light_intensity = fmaxf(0.0f, fminf(1.0f, light_intensity));

            // Apply light intensity to base color
            color_t base_color = (color_t){ .a = 0xFF, .r = 0xFF, .g = 0xFF, .b = 0xFF }; // White color
            vertex_colors[j] = light_apply_intensity_color(base_color, light_intensity);

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
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);


        // calculate the shade intensity based on how aligned is the face normal and the light ray
        float light_intensity_factor = vec3_dot(normal, light.direction);

        // THIS WAS A FUNCTION USED FOR FLAT SHADING
        // calculate the triangle color based on the light angle
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
        

         // **Create the triangle with per-vertex colors**
        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y },
            },
            .point_colors = {
                vertex_colors[0],
                vertex_colors[1],
                vertex_colors[2],
            },
            .color = triangle_color // Not used when using per-vertex colors
        };

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
        // triangle.point_colors[0] = (color_t){ .a = 0xFF, .r = 0xFF, .g = 0x00, .b = 0x00 };  // Red
        // triangle.point_colors[1] = (color_t){ .a = 0xFF, .r = 0x00, .g = 0xFF, .b = 0x00 };  // Green
        // triangle.point_colors[2] = (color_t){ .a = 0xFF, .r = 0x00, .g = 0x00, .b = 0xFF };  // Blue

        if (backface_cull_check(triangle.points[0], triangle.points[1], triangle.points[2]))
        {
            if (render_method == RENDER_FILL_TRIANGLE)
            {
                triangle_fill(triangle.points[0], triangle.points[1], triangle.points[2], triangle.color);
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
                triangle_fill(triangle.points[0], triangle.points[1], triangle.points[2], triangle.color);
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

        // TODO: MAKE RENDER METHOD FOR BARYCENTRIC COORDINATES
            
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
    array_free(mesh.normals);
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