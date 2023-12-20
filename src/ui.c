#include <stdlib.h>
#include "icons.c"
#include "ui.h"

#define ICON_WIDTH 50
#define ICON_HEIGHT 50
#define BAR_HEIGHT 70
#define SPACING 30
#define MARGIN 10

static SDL_Texture* ptexture_icons;
static SDL_Rect ui_bg_rect;
static SDL_Rect camera_dst_rect = {MARGIN,
                                   MARGIN,
                                   ICON_WIDTH,
                                   ICON_HEIGHT};
static SDL_Rect hlr_dst_rect = {MARGIN + ICON_WIDTH + SPACING,
                                MARGIN,
                                ICON_WIDTH,
                                ICON_HEIGHT};

const SDL_Rect orbit_src_rect = {0, 0, ICON_WIDTH, ICON_HEIGHT};
const SDL_Rect eye_src_rect = {0, ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT};
const SDL_Rect hlr_on_src_rect = {ICON_WIDTH, 0, ICON_WIDTH, ICON_HEIGHT};
const SDL_Rect hlr_off_src_rect = {ICON_WIDTH, ICON_HEIGHT, ICON_WIDTH, ICON_HEIGHT};

void init_ui(int win_height, int win_width, SDL_Renderer* prenderer){
    // Background
    ui_bg_rect.x = 0;
    ui_bg_rect.y = win_height-BAR_HEIGHT;
    ui_bg_rect.w = win_width;
    ui_bg_rect.h = BAR_HEIGHT;

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
    camera_dst_rect.x += ui_bg_rect.x;
    camera_dst_rect.y += ui_bg_rect.y;

    hlr_dst_rect.x += ui_bg_rect.x;
    hlr_dst_rect.y += ui_bg_rect.y;
};

void draw_ui(SDL_Renderer* prenderer, bool orbit_mode, bool hidden_removed){
    // Background
    SDL_SetRenderDrawColor(prenderer, 0, 0, 0, 255);
    SDL_RenderFillRect(prenderer, &ui_bg_rect);
    // Camera
    if (orbit_mode)
        SDL_RenderCopy(prenderer, ptexture_icons, &orbit_src_rect, &camera_dst_rect);
    else
        SDL_RenderCopy(prenderer, ptexture_icons, &eye_src_rect, &camera_dst_rect);
    // HLR
    if (hidden_removed)
        SDL_RenderCopy(prenderer, ptexture_icons, &hlr_on_src_rect, &hlr_dst_rect);
    else
        SDL_RenderCopy(prenderer, ptexture_icons, &hlr_off_src_rect, &hlr_dst_rect);
}
