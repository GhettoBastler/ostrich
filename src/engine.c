#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "transforms.h"
#include "primitives.h"
#include "scene.h"
#include "camera.h"

#define FPS 60
#define EXPORT_PATH "export.bmp"

void draw_line(Uint32* ppixels, Edge2D edge){
    int x0 = (int) edge.a.x,
        y0 = (int) edge.a.y,
        x1 = (int) edge.b.x,
        y1 = (int) edge.b.y;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx+dy,
        e2;

    for (;;){
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
            ppixels[x0 + WIDTH * y0] = 0xFF000000;

        if (x0 == x1 && y0 == y1) break;
        e2 = 2*err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw(Uint32* ppixels, Mesh2D* pmesh, SDL_Texture* ptexture, SDL_Renderer* prenderer){
    int pitch = WIDTH * sizeof(Uint32);
    SDL_LockTexture(ptexture, NULL, (void**) &ppixels, &pitch);
    //Clear pixels
    for (int i = 0; i < HEIGHT * WIDTH; i++){
        ppixels[i] = 0xFFFFFFFF;
    }
    //Draw lines
    for (int i = 0; i < pmesh->size; i++){
        draw_line(ppixels, pmesh->edges[i]);
    }
    SDL_UnlockTexture(ptexture);
    SDL_RenderCopy(prenderer, ptexture, NULL, NULL);
    SDL_RenderPresent(prenderer);
}

void export(SDL_Renderer* prenderer){
    //https://discourse.libsdl.org/t/save-image-from-render/21009/2
    SDL_Surface* psshot = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    if (psshot == NULL){
        fprintf(stderr, "Couldn't create a surface to export image\n");
    } else {
        SDL_RenderReadPixels(prenderer, NULL, 0, psshot->pixels, psshot->pitch);
        SDL_SaveBMP(psshot, EXPORT_PATH);
        printf("File exported as %s\n", EXPORT_PATH);
        SDL_FreeSurface(psshot);
    }
}

int main(int argc, char **argv){
    // SDL initialization
    SDL_Window* pwindow = NULL;
    SDL_Renderer* prenderer = NULL;
    SDL_Texture* ptexture = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    pwindow = SDL_CreateWindow("SDL Example",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            0);

    if (pwindow == NULL) {
        fprintf(stderr, "SDL window failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    prenderer = SDL_CreateRenderer(pwindow,
            -1,
            0);

    if (prenderer == NULL) {
        fprintf(stderr, "SDL renderer failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if (ptexture == NULL) {
        fprintf(stderr, "SDL texture failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    // Initializing main loop
    Uint32* ppixels = (Uint32*) malloc(WIDTH * HEIGHT * sizeof(Uint32));

    if (ppixels == NULL){
        fprintf(stderr, "Couldn't allocate memory for frame buffer\n");
        return 1;
    }

    // Creating scene
    Mesh3D* pscene = make_scene();

    // Initializing camera
    Camera cam = make_camera(0, 0, 100, M_PI/6, 0, 0, 800);

    // Creating a buffer for the 2D projection
    Mesh2D* pbuffer = (Mesh2D*) malloc(sizeof(Mesh2D) + pscene->size * sizeof(Edge2D));
    if (pbuffer == NULL){
        fprintf(stderr, "Couldn't create the buffer for the 2D projection\n");
        return 1;
    }

    // Main loop
    Uint32 time_start, delta;
    SDL_Event event;

    bool should_draw;
    bool is_stopped = false;
    bool button_pressed = false;
    bool shift_pressed = false;
    int prev_x, prev_y;

    float dist = 100.;
    float focal_length = 800.;
    Point3D rotation, translation;
    update_rotation_matrix(&cam);

    project_mesh(pbuffer, pscene, &cam);
    draw(ppixels, pbuffer, ptexture, prenderer);

    while (!is_stopped){
        should_draw = false;
        time_start = SDL_GetTicks();
        //Processing inputs
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    is_stopped = true;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT){
                        button_pressed = event.button.state == SDL_PRESSED;
                        if (button_pressed){
                            prev_x = event.button.x;
                            prev_y = event.button.y;
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    if (button_pressed){
                        if (shift_pressed){
                            cam.translation.x += (float)(event.motion.x - prev_x)/5;
                            cam.translation.y += (float)(event.motion.y - prev_y)/5;
                        } else {
                            cam.rotation.y += -(float)(event.motion.x - prev_x)/100;
                            cam.rotation.z += (float)(event.motion.y - prev_y)/100;
                            // Update matrix components
                            update_rotation_matrix(&cam);
                        }
                        prev_x = event.motion.x;
                        prev_y = event.motion.y;
                        should_draw = true;
                    }
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LSHIFT) {
                        shift_pressed = event.key.state == SDL_PRESSED;
                    } else if (event.key.keysym.sym == SDLK_e && event.key.type == SDL_KEYDOWN){
                        export(prenderer);
                    }
                    break;
                
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        if (shift_pressed){
                            cam.focal_length += 5;
                        } else {
                            cam.translation.z += 1;
                        }
                    } else {
                        if (shift_pressed){
                            cam.focal_length -= 5;
                        } else {
                            cam.translation.z -= 1;
                        }
                    }
                    should_draw = true;
                    break;
            }
        }

        //Drawing
        if (should_draw){
            project_mesh(pbuffer, pscene, &cam);
            draw(ppixels, pbuffer, ptexture, prenderer);
        }

        //FPS caping
        delta = SDL_GetTicks() - time_start;
        if (delta == 0 || 1000 / delta > FPS) {
            SDL_Delay((1000 / FPS) - delta);
        }
    }

    // Freeing
    free(ppixels);
    free(pbuffer);
    free(pscene);

    SDL_DestroyTexture(ptexture);
    SDL_DestroyRenderer(prenderer);
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;
}
