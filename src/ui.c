#include <stdlib.h>
#include "icons.c"
#include "ui.h"

static SDL_Texture* ptexture_icons;
static SDL_Rect ui_bg_rect;
static SDL_Rect camera_dst_rect;
const SDL_Rect orbit_src_rect = {0, 0, 50, 50};
const SDL_Rect eye_src_rect = {0, 50, 50, 50};

void init_ui(int win_height, int win_width, SDL_Renderer* prenderer){
    // Background
    ui_bg_rect.x = 0;
    ui_bg_rect.y = win_height-70;
    ui_bg_rect.w = win_width;
    ui_bg_rect.h = 70;

    // Icons
    // Load sheet
    SDL_Surface* psurface_icons = SDL_CreateRGBSurfaceFrom((void*)icons.pixel_data,
            icons.width,
            icons.height,
            icons.bytes_per_pixel * 8,
            icons.bytes_per_pixel * icons.width,
            0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    ptexture_icons = SDL_CreateTextureFromSurface(prenderer, psurface_icons);
    free(psurface_icons);

    // Set render positions
    camera_dst_rect.x = ui_bg_rect.x + 10;
    camera_dst_rect.y = ui_bg_rect.y + 10;
    camera_dst_rect.w = 50;
    camera_dst_rect.h = 50;
};

void draw_ui(SDL_Renderer* prenderer, bool orbit_mode){
    // Background
    SDL_SetRenderDrawColor(prenderer, 0, 0, 0, 255);
    SDL_RenderFillRect(prenderer, &ui_bg_rect);
    if (orbit_mode)
        SDL_RenderCopy(prenderer, ptexture_icons, &orbit_src_rect, &camera_dst_rect);
    else
        SDL_RenderCopy(prenderer, ptexture_icons, &eye_src_rect, &camera_dst_rect);
}

void destroy_ui(){
    //free(ptexture_eye);
    //free(psurface_eye);
}
