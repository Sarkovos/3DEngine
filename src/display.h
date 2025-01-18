#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "triangle.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS) //How many miliseconds a frame should take

//************************
//Global Variables
//************************

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* frame_buffer;
extern float* z_buffer;
extern SDL_Texture* frame_buffer_texture;

extern int window_width;
extern int window_height;

extern float aspect;
extern float fov;

bool initialize_window(void);
void render_frame_buffer(void);
void clear_frame_buffer (uint32_t color);
void clear_z_buffer();
void destroy_window(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void bres_draw_line(int x0, int y0, int x1, int y1, uint32_t color);







#endif