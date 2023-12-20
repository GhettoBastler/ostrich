#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "engine.h"

#define ICON_WIDTH 50
#define ICON_HEIGHT 50
#define BAR_HEIGHT 70
#define SPACING 30
#define MARGIN 10

void init_ui(int win_height, int win_width, SDL_Renderer* prenderer);
void draw_ui(SDL_Renderer* prenderer, bool orbit_mode, bool hidden_removed);
void process_ui_click(int mouse_x, int mouse_y, Uint32 mousestate, EngineState* pstate);

#endif
