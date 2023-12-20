#include "ui.h"
#include "icons.c"

static bool clicked;
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


bool is_in_rect(int x, int y, SDL_Rect* rect);


bool is_in_rect(int x, int y, SDL_Rect* prect){
    return (x >= prect->x && x <= prect->x + prect->w &&
            y >= prect->y && y <= prect->y + prect->h);
}

void init_ui(int win_height, int win_width, SDL_Renderer* prenderer){
    // Mouse
    clicked = false;
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

void process_ui_click(int mouse_x, int mouse_y, Uint32 mousestate, EngineState* pstate){
    if (is_in_rect(mouse_x, mouse_y, &ui_bg_rect)){
        // Mouse is here!
        if (mousestate & SDL_BUTTON(1)){
            // Oh boy, it's a click!
            // Wait, is the button just being held ?
            if (!clicked)
                // Nope, it's a genuine click!
                // What button is being clicked ?
                if (is_in_rect(mouse_x, mouse_y, &camera_dst_rect)){
                    // Toggle orbit
                    printf("Orbit toggled\n");
                    pstate->orbit = !pstate->orbit;
                } else if (is_in_rect(mouse_x, mouse_y, &hlr_dst_rect)){
                    // Trigger HLR
                    printf("HLR triggered\n");
                    pstate->do_hlr = true;
                }
                clicked = true;
        } else {
            clicked = false;
        }
    }
}
