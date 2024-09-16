#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

//************************
//Global Variables
//************************
bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

/*Initialize an SDL Window and verify it works properly*/
bool initialize_window(void)
{
    //Iniitalize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) //SDL Constant to init everything
    {
        fprintf(stderr, "Error initializing SDL. \n");
        return false;
    }

    /*args: Title, xPos of window, yPos of window, width, height, flags*/
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
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

    return true;
}

void setup(void) 
{

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
        
    }
}

void update(void)
{

}


void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
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


    return 0;
}