#include "display.h"
/*Display.c handles the SDL side of our engine*/

//************************
//Global Variables
//************************
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* frame_buffer = NULL;
SDL_Texture* frame_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;


bool is_running = false;

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
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    /*args: Title, xPos of window, yPos of window, width, height, flags*/
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
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
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
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


/*Free all memory*/
void destroy_window(void)
{
    free(frame_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
