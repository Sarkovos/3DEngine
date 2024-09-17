#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

//************************
//Global Variables
//************************

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* frame_buffer;
extern SDL_Texture* frame_buffer_texture;

int window_width;
int window_height;

bool initialize_window(void);
void render_frame_buffer(void);
void clear_frame_buffer (uint32_t color);
void destroy_window(void);

#endif