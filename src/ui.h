#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void init_ui(int win_height, int win_width, SDL_Renderer* prenderer);
void draw_ui(SDL_Renderer* prenderer, bool orbit_mode, bool hidden_removed);
void destroy_ui();

#endif
