#include <stdlib.h>
#include "eye.c"
#include "orbit.c"
#include "ui.h"

static SDL_Surface* psurface_eye;
static SDL_Texture* ptexture_eye;
static SDL_Surface* psurface_orbit;
static SDL_Texture* ptexture_orbit;

void load_ui(SDL_Renderer* prenderer){
    psurface_eye = SDL_CreateRGBSurfaceFrom((void*)eye_icon.pixel_data, eye_icon.width, eye_icon.height, eye_icon.bytes_per_pixel * 8, eye_icon.bytes_per_pixel * eye_icon.width, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    psurface_orbit = SDL_CreateRGBSurfaceFrom((void*)orbit_icon.pixel_data, orbit_icon.width, orbit_icon.height, orbit_icon.bytes_per_pixel * 8, orbit_icon.bytes_per_pixel * orbit_icon.width, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    ptexture_eye = SDL_CreateTextureFromSurface(prenderer, psurface_eye);
    ptexture_orbit = SDL_CreateTextureFromSurface(prenderer, psurface_orbit);
};

void draw_ui(SDL_Renderer* prenderer, int window_height, bool orbit_mode){
    SDL_Rect dst_rect = {10, window_height-60, 50, 50};
    if (orbit_mode)
        SDL_RenderCopy(prenderer, ptexture_orbit, NULL, &dst_rect);
    else
        SDL_RenderCopy(prenderer, ptexture_eye, NULL, &dst_rect);
}

void destroy_ui(){
    free(ptexture_eye);
    free(psurface_eye);
}
