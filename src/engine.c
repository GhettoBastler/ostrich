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
#define LINE_COLOR_1 0xFFF4115D
#define LINE_COLOR_2 0xFF0296F2
#define BG_COLOR 0xFF111111

//Edge2D cap_edge(Edge2D edge){
ProjectedEdge cap_edge(ProjectedEdge edge){
    Point2D a = edge.edge2D.a;
    Point2D b = edge.edge2D.b;

    Point3D a3 = edge.edge3D.a;
    Point3D b3 = edge.edge3D.b;
    float dx3 = b3.x - a3.x;
    float dy3 = b3.y - a3.y;
    float dz3 = b3.z - a3.z;

    float dx = b.x - a.x;
    float dy = b.y - a.y;

    float ratio;

    if (dx != 0){
        if (a.x < 0){
            ratio = -(a.x / dx);
            a.y = -(a.x / dx) * dy + a.y;
            a.x = 0;

            a3.x = ratio * dx3 + a3.x;
            a3.y = ratio * dy3 + a3.y;
            a3.z = ratio * dz3 + a3.z;
        } else if (a.x > WIDTH){
            ratio = -(a.x - WIDTH)/ dx;
            a.y = -((a.x - WIDTH) / dx) * dy + a.y;
            a.x = WIDTH;

            a3.x = ratio * dx3 + a3.x;
            a3.y = ratio * dy3 + a3.y;
            a3.z = ratio * dz3 + a3.z;
        }

        if (b.x < 0){
            ratio = -b.x / dx;
            b.y = -(b.x / dx) * dy + b.y;
            b.x = 0;

            b3.x = ratio * dx3 + b3.x;
            b3.y = ratio * dy3 + b3.y;
            b3.z = ratio * dz3 + b3.z;
        } else if (b.x > WIDTH){
            ratio = -(b.x - WIDTH)/ dx;
            b.y = -((b.x - WIDTH) / dx) * dy + b.y;
            b.x = WIDTH;

            b3.x = ratio * dx3 + b3.x;
            b3.y = ratio * dy3 + b3.y;
            b3.z = ratio * dz3 + b3.z;
        }
    }
    
    if (dy != 0){
        if (a.y < 0){
            ratio = -(a.y / dy);
            a.x = -(a.y / dy) * dx + a.x;
            a.y = 0;

            a3.x = ratio * dx3 + a3.x;
            a3.y = ratio * dy3 + a3.y;
            a3.z = ratio * dz3 + a3.z;
        } else if (a.y > HEIGHT){
            ratio = -(a.y - HEIGHT) / dy;
            a.x = -((a.y - HEIGHT) / dy) * dx + a.x;
            a.y = HEIGHT;

            a3.x = ratio * dx3 + a3.x;
            a3.y = ratio * dy3 + a3.y;
            a3.z = ratio * dz3 + a3.z;
        }

        if (b.y < 0){
            ratio = -b.y/ dy;
            b.x = -(b.y / dy) * dx + b.x;
            b.y = 0;

            b3.x = ratio * dx3 + b3.x;
            b3.y = ratio * dy3 + b3.y;
            b3.z = ratio * dz3 + b3.z;
        } else if (b.y > HEIGHT){
            ratio = -(b.y - HEIGHT) / dy;
            b.x = -((b.y - HEIGHT) / dy) * dx + b.x;
            b.y = HEIGHT;

            b3.x = ratio * dx3 + b3.x;
            b3.y = ratio * dy3 + b3.y;
            b3.z = ratio * dz3 + b3.z;
        }
    }

    //Edge2D res = {a, b};
    Edge2D res2D = {a, b};
    Edge3D res3D = {a3, b3};
    ProjectedEdge res = {res2D, res3D};
    return res;
}

//void draw_line(Uint32* ppixels, Edge2D edge){
void draw_line(Uint32* ppixels, ProjectedEdge edge, TriangleMesh* pmesh, bool draw_hidden){

    //Edge2D capped = cap_edge(edge.edge2D);
    ProjectedEdge proj_capped = cap_edge(edge);
    Edge2D capped = proj_capped.edge2D;

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

    float ratio;
    float span = sqrt(pow(dx, 2) + pow(dy, 2));
    int x_init = x0;
    int y_init = y0;

    for (;;){
        ratio = sqrt(pow(x0 - x_init, 2) + pow(y0 - y_init, 2)) / span;
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
            if (draw_hidden)
                ppixels[x0 + WIDTH * y0] = LINE_COLOR_1;
            else if (point_is_visible(proj_capped.edge3D, ratio, pmesh))
                    ppixels[x0 + WIDTH * y0] = LINE_COLOR_2;

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

void update_texture(Uint32* ppixels, ProjectedMesh* pmesh, SDL_Texture* ptexture, TriangleMesh* ptri_mesh, bool draw_hidden){
    int pitch = WIDTH * sizeof(Uint32);
    SDL_LockTexture(ptexture, NULL, (void**) &ppixels, &pitch);
    //Clear pixels
    for (int i = 0; i < HEIGHT * WIDTH; i++){
        ppixels[i] = BG_COLOR;
    }
    //Draw lines
    for (int i = 0; i < pmesh->size; i++){
        draw_line(ppixels, pmesh->edges[i], ptri_mesh, draw_hidden);
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
    TriangleMesh* pculled_tri;

    // Initializing camera
    Camera cam = make_camera(0, 0, 0, 0, 0, 0, 800);

    // Creating a buffer for the 2D projection
    ProjectedMesh* pbuffer = (ProjectedMesh*) malloc(sizeof(ProjectedMesh) + pscene->size * sizeof(ProjectedEdge) * 3);
    check_allocation(pbuffer, "Couldn\'t allocate memory for 2D projection buffer\n");

    // Main loop
    Uint32 time_start, delta;
    SDL_Event event;

    bool reproject, do_hidden;
    bool is_stopped = false;
    int prev_x, prev_y;

    pculled_tri = project_tri_mesh(pbuffer, pscene, &cam);
    update_texture(ppixels, pbuffer, ptexture, pculled_tri, true);
    free(pculled_tri);
    draw(ptexture, prenderer);

    Point3D rotation, translation;
    do_hidden = false;

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
            translation.z = -1;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_S]) {
            translation.z = 1;
            reproject = true;
        }
        if (kbstate[SDL_SCANCODE_A]) {
            translation.x = 1;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_D]) {
            translation.x = -1;
            reproject = true;
        }
        if (kbstate[SDL_SCANCODE_Q]) {
            translation.y = 1;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_E]) {
            translation.y = -1;
            reproject = true;
        }
        if (kbstate[SDL_SCANCODE_R]) {
            do_hidden = true;
        }

        //Projecting
        if (do_hidden){
            do_hidden = false;
            update_transform_matrix(cam.transform_mat, rotation, translation);
            pculled_tri = project_tri_mesh(pbuffer, pscene, &cam);
            update_texture(ppixels, pbuffer, ptexture, pculled_tri, false);
            free(pculled_tri);
        } else if (reproject) {
            update_transform_matrix(cam.transform_mat, rotation, translation);
            pculled_tri = project_tri_mesh(pbuffer, pscene, &cam);
            update_texture(ppixels, pbuffer, ptexture, pculled_tri, true);
            free(pculled_tri);
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
