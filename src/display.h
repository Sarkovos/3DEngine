#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

//************************
//Global Variables
//************************

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* frame_buffer;
extern SDL_Texture* frame_buffer_texture;

extern int window_width;
extern int window_height;

bool initialize_window(void);
void render_frame_buffer(void);
void clear_frame_buffer (uint32_t color);
void destroy_window(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

#endif