#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void load_ui(SDL_Renderer* prenderer);
void draw_ui(SDL_Renderer* prenderer, int win_height, int win_width, bool orbit_mode);
void destroy_ui();

#endif
