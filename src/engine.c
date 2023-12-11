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
#define LINE_COLOR 0xFFFCE46C
#define BG_COLOR 0xFF111111

Edge2D cap_edge(Edge2D edge){
    Point2D a = edge.a;
    Point2D b = edge.b;

    float dx = b.x - a.x;
    float dy = b.y - a.y;

    if (dx != 0){
        if (a.x < 0){
            a.y = -(a.x / dx) * dy + a.y;
            a.x = 0;
        } else if (a.x > WIDTH){
            a.y = -((a.x - WIDTH) / dx) * dy + a.y;
            a.x = WIDTH;
        }

        if (b.x < 0){
            b.y = -(b.x / dx) * dy + b.y;
            b.x = 0;
        } else if (b.x > WIDTH){
            b.y = -((b.x - WIDTH) / dx) * dy + b.y;
            b.x = WIDTH;
        }
    }
    
    if (dy != 0){
        if (a.y < 0){
            a.x = -(a.y / dy) * dx + a.x;
            a.y = 0;
        } else if (a.y > HEIGHT){
            a.x = -((a.y - HEIGHT) / dy) * dx + a.x;
            a.y = HEIGHT;
        }

        if (b.y < 0){
            b.x = -(b.y / dy) * dx + b.x;
            b.y = 0;
        } else if (b.y > HEIGHT){
            b.x = -((b.y - HEIGHT) / dy) * dx + b.x;
            b.y = HEIGHT;
        }
    }

    Edge2D res = {a, b};
    return res;
}

void draw_line(Uint32* ppixels, Edge2D edge){

    Edge2D capped = cap_edge(edge);

    int x0 = (int) capped.a.x,
        y0 = (int) capped.a.y,
        x1 = (int) capped.b.x,
        y1 = (int) capped.b.y;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx+dy,
        e2;

    for (;;){
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
            ppixels[x0 + WIDTH * y0] = LINE_COLOR;

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

void update_texture(Uint32* ppixels, Mesh2D* pmesh, SDL_Texture* ptexture){
    int pitch = WIDTH * sizeof(Uint32);
    SDL_LockTexture(ptexture, NULL, (void**) &ppixels, &pitch);
    //Clear pixels
    for (int i = 0; i < HEIGHT * WIDTH; i++){
        ppixels[i] = BG_COLOR;
    }
    //Draw lines
    for (int i = 0; i < pmesh->size; i++){
        draw_line(ppixels, pmesh->edges[i]);
    }
    SDL_UnlockTexture(ptexture);
}

void draw(SDL_Texture* ptexture, SDL_Renderer* prenderer){
    SDL_RenderCopy(prenderer, ptexture, NULL, NULL);
    SDL_RenderPresent(prenderer);
}

void check_allocation(void* pointer, char* message){
    if (pointer == NULL){
        fprintf(stderr, message);
        exit(1);
    }
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

    check_allocation(pwindow, "SDL window failed to initialize\n");

    prenderer = SDL_CreateRenderer(pwindow, -1, 0);
    check_allocation(prenderer, "SDL renderer failed to initialize\n");

    ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    check_allocation(ptexture, "SDL texture failed to initialize\n");

    Uint32* ppixels = (Uint32*) malloc(WIDTH * HEIGHT * sizeof(Uint32));
    check_allocation(ppixels, "Couldn\'t allocate memory for frame buffer\n");

    // Keyboard
    const Uint8* kbstate = SDL_GetKeyboardState(NULL);
 
    // Mouse
    Uint32 mousestate;
    int mouse_x, mouse_y;

    // Initializing main loop
    // Creating scene
    //Mesh3D* pscene = make_scene();
    TriangleMesh* pscene = tri_make_scene();

    // Initializing camera
    Camera cam = make_camera(0, 0, 0, 0, 0, 0, 800);

    // Creating a buffer for the 2D projection
    // Mesh2D* pbuffer = (Mesh2D*) malloc(sizeof(Mesh2D) + pscene->size * sizeof(Edge2D));
    Mesh2D* pbuffer = (Mesh2D*) malloc(sizeof(Mesh2D) + pscene->size * sizeof(Edge2D) * 3);
    check_allocation(pbuffer, "Couldn\'t allocate memory for 2D projection buffer\n");

    // Main loop
    Uint32 time_start, delta;
    SDL_Event event;

    bool reproject;
    bool is_stopped = false;
    int prev_x, prev_y;

    //project_mesh(pbuffer, pscene, &cam);
    project_tri_mesh(pbuffer, pscene, &cam);
    update_texture(ppixels, pbuffer, ptexture);
    draw(ptexture, prenderer);

    Point3D rotation, translation;

    while (!is_stopped){
        translation.x = translation.y = translation.z = 0;
        rotation.x = rotation.y = rotation.z = 0;
        reproject = false;
        time_start = SDL_GetTicks();
        
        //Processing inputs
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    is_stopped = true;
                    break;

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        cam.focal_length += 5;
                    } else {
                        cam.focal_length -= 5;
                    }
                    reproject = true;
                    break;
            }

        }

        // Mouse
        mousestate = SDL_GetMouseState(&mouse_x, &mouse_y);
        if (mousestate & SDL_BUTTON(1)){
            rotation.y = -(float)(mouse_x - prev_x)/500;
            rotation.x = -(float)(mouse_y - prev_y)/500;
            reproject = true;
        } else if (mousestate & SDL_BUTTON(2)){
            rotation.z = (float)(mouse_x - prev_x)/500;
            reproject = true;
        }
        prev_x = mouse_x;
        prev_y = mouse_y;

        // Keyboard

        if (kbstate[SDL_SCANCODE_W]) {
            translation.z = -0.05;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_S]) {
            translation.z = 0.05;
            reproject = true;
        }
        if (kbstate[SDL_SCANCODE_A]) {
            translation.x = 0.05;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_D]) {
            translation.x = -0.05;
            reproject = true;
        }
        if (kbstate[SDL_SCANCODE_Q]) {
            translation.y = 0.05;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_E]) {
            translation.y = -0.05;
            reproject = true;
        }

        //Projecting
        if (reproject){
            update_transform_matrix(cam.transform_mat, rotation, translation);
            //project_mesh(pbuffer, pscene, &cam);
            project_tri_mesh(pbuffer, pscene, &cam);
            update_texture(ppixels, pbuffer, ptexture);
        }

        //Drawing
        draw(ptexture, prenderer);

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
