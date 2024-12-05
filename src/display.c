#include "display.h"
#include <math.h>
/*Display.c handles the SDL side of our engine, as well as other details like line drawing
and pixel drawing*/

//************************
//Global Variables
//************************
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* frame_buffer = NULL;
float* z_buffer = NULL;
SDL_Texture* frame_buffer_texture = NULL;
int window_width = 800;
int window_height = 800;

/*Initialize an SDL Window and verify it works properly*/
bool initialize_window(void)
{
    //Iniitalize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) //SDL Constant to init everything
    {
        fprintf(stderr, "Error initializing SDL. \n");
        return false;
    }

    //USe SDL to query what is the fullscreen max width and height
    // SDL_DisplayMode display_mode;
    // SDL_GetCurrentDisplayMode(0, &display_mode);

    // window_width = display_mode.w;
    // window_height = display_mode.h;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_RESIZABLE  
    );

    if(!window)
    {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }

    //Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error Creating SDL Renderer.\n");
        return false;
    }
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

/*Draws pixel at x,y position in our frame buffer*/
void draw_pixel(int x, int y, uint32_t color)
{

    // Invert the y-coordinate for top-left origin systems
    // places points with y = 0 at the bottom of the screen
    
    int screen_y = window_height - y - 1;
    if (x < window_width && screen_y < window_height)
    {
        frame_buffer[(window_width * screen_y) + x] = color;
    }
    
}

//DDA Algorithm
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

//Bresenhams algorithm
//As the algorithm plots in the x direction, error will increase in the positive
//direction eventually making e2 greater than dx, where it will stop plotting in the
//x direction. As it goes in the y direction, error will increment in the negative direction,
//eventually making e2 less than
void bres_draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{

    //calculate change in x and y
    int dx = abs(x1 - x0); //always positive
    int dy = -abs(y1 - y0); //always negative

    int stepX = x0 < x1 ? 1 : -1; //Step x left or right
    int stepY = y0 < y1 ? 1 : -1; //Step y left or right
    int error = dx + dy; //start state for difference between dx and dy

    while (x0 != x1 || y0 != y1)
    {
        draw_pixel(x0, y0, color);
        int doubleError = 2 * error;
        if (doubleError > dy)
        {
            error += dy;
            x0 += stepX; //move x0 in the x direction setup in stepX
        }
        if (doubleError < dx)
        {
            error += dx;
            y0 += stepY; //move y0 in the y direction setup in stepY
        }

        
    }
    draw_pixel(x1, y1, color);
}



/*Copy the contents of the frame buffer to the SDL texture to be displayed*/
void render_frame_buffer(void)
{
    SDL_UpdateTexture(
        frame_buffer_texture,
        NULL,
        frame_buffer,
        (int)(window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, frame_buffer_texture, NULL, NULL);
}

/*Clear the frame buffer with some arbitrary color*/
void clear_frame_buffer (uint32_t color)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            frame_buffer[(window_width * y) + x] = color;
        }
    }
}

void clear_z_buffer (void)
{
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            z_buffer[(window_width * y) + x] = 1.0;
        }
    }
}




/*Free all memory*/
void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
