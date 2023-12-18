#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "transforms.h"
#include "primitives.h"
#include "scene.h"
#include "camera.h"
#include "vect.h"

#define FPS 60
#define EXPORT_PATH "export.bmp"
#define LINE_COLOR_1 0xFFF4115D
#define LINE_COLOR_2 0xFF0296F2
#define BG_COLOR 0xFF111111

void draw_line(Uint32* ppixels, ProjectedEdge edge, TriangleMesh* pmesh, bool draw_hidden, Camera* pcam){

    Edge2D capped = edge.edge2D;
    capped.a.x = (capped.a.x + pcam->width/2)*SCALE;
    capped.a.y = (capped.a.y + pcam->height/2)*SCALE;
    capped.b.x = (capped.b.x + pcam->width/2)*SCALE;
    capped.b.y = (capped.b.y + pcam->height/2)*SCALE;

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

    int i=0;

    if (!draw_hidden){
        // Check when the line's bounding box gets obstructed
        Point3D tri_bbox_min, tri_bbox_max,
                edge_bbox_min = pt_min(edge.edge3D.a, edge.edge3D.b),
                edge_bbox_max = pt_max(edge.edge3D.a, edge.edge3D.b);
        Triangle curr_tri;

        for (i=0; i < pmesh->size; i++){
            curr_tri = pmesh->triangles[i];

            tri_bbox_min = pt_min(pt_min(curr_tri.a, curr_tri.b), curr_tri.c);
            tri_bbox_max = pt_max(pt_max(curr_tri.a, curr_tri.b), curr_tri.c);

            // If the edge's bbox is completely in front of this triangle's bounding box,
            // it is not hidden by this triangle, nor by any other
            if (edge_bbox_max.z < tri_bbox_min.z){
                i = pmesh->size - 1;
                break;
            }

            // If edge's bbox is projected completely outside of the triangle bounding box,
            // the triangle doesn't hide it
            if (
                (
                 (edge_bbox_max.x * tri_bbox_min.z / edge_bbox_max.z < tri_bbox_min.x) &&
                 (edge_bbox_max.x * tri_bbox_max.z / edge_bbox_max.z < tri_bbox_min.x)
                ) || (
                 (edge_bbox_min.x * tri_bbox_min.z / edge_bbox_max.z > tri_bbox_max.x) &&
                 (edge_bbox_min.x * tri_bbox_max.z / edge_bbox_max.z > tri_bbox_max.x)
                ) || (
                 (edge_bbox_max.y * tri_bbox_min.z / edge_bbox_max.z < tri_bbox_min.y) &&
                 (edge_bbox_max.y * tri_bbox_max.z / edge_bbox_max.z < tri_bbox_min.y)
                ) || (
                 (edge_bbox_min.y * tri_bbox_min.z / edge_bbox_max.z > tri_bbox_max.y) &&
                 (edge_bbox_min.y * tri_bbox_max.z / edge_bbox_max.z > tri_bbox_max.y)
                )
            )
                continue;

            // At this point, we know that the edge's bounding box is in the shadow of
            // this triangle's bounding box. Then we do the modified Bresenham with
            // the triangle list starting from this point
            break;
        }
    };

    // Bresenham
    for (;;){
        ratio = sqrt(pow(x0 - x_init, 2) + pow(y0 - y_init, 2)) / span;
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
            if (draw_hidden)
                ppixels[x0 + WIDTH * y0] = LINE_COLOR_1;
            else if (point_is_visible(edge.edge3D, ratio, pmesh, i))
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

void update_texture(Uint32* ppixels, ProjectedMesh* pmesh, SDL_Texture* ptexture, TriangleMesh* ptri_mesh, bool draw_hidden, Camera* pcam){
    int pitch = WIDTH * sizeof(Uint32);
    SDL_LockTexture(ptexture, NULL, (void**) &ppixels, &pitch);
    //Clear pixels
    for (int i = 0; i < HEIGHT * WIDTH; i++){
        ppixels[i] = BG_COLOR;
    }
    //Draw lines
    for (int i = 0; i < pmesh->size; i++){
        draw_line(ppixels, pmesh->edges[i], ptri_mesh, draw_hidden, pcam);
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
    TriangleMesh* pscene = tri_make_scene();
    TriangleMesh* pculled_tri;

    // Initializing camera
    Camera cam = make_camera(WIDTH/SCALE, HEIGHT/SCALE, 800/SCALE);
    float orbit_radius = 0;
    bool orbit_pressed = false;
    bool orbit = false;
    bool shift_pressed = false;

    // Creating a buffer for the 2D projection
    ProjectedMesh* pbuffer = (ProjectedMesh*) malloc(sizeof(ProjectedMesh) + pscene->size * sizeof(ProjectedEdge) * 3);
    check_allocation(pbuffer, "Couldn\'t allocate memory for 2D projection buffer\n");

    // Main loop
    Uint32 time_start, delta;
    SDL_Event event;

    bool reproject, do_hidden, captured;
    bool is_stopped = false;
    int prev_x, prev_y;

    pculled_tri = project_tri_mesh(pbuffer, pscene, &cam);
    update_texture(ppixels, pbuffer, ptexture, pculled_tri, true, &cam);
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
            if (shift_pressed){
                translation.x = (float)(mouse_x - prev_x)/2;
                translation.y = (float)(mouse_y - prev_y)/2;
            } else {
                rotation.y = -(float)(mouse_x - prev_x)/500;
                rotation.x = (float)(mouse_y - prev_y)/500;
            }
            reproject = true;
        } else if (mousestate & SDL_BUTTON(3)){
            rotation.z = (float)(mouse_x - prev_x)/500;
            reproject = true;
        }

        prev_x = mouse_x;
        prev_y = mouse_y;

        // Keyboard

        if (kbstate[SDL_SCANCODE_W]) {
            translation.z = -1;
            orbit_radius += -1;
            reproject = true;
        } else if (kbstate[SDL_SCANCODE_S]) {
            translation.z = 1;
            orbit_radius += 1;
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
        if (kbstate[SDL_SCANCODE_O]) {
            if (!orbit_pressed){
                orbit_pressed = true;
                orbit = !orbit;
                printf("Orbit mode toggled\n");
            }
        } else {
            orbit_pressed = false;
        }
        if (kbstate[SDL_SCANCODE_SPACE]) {
            if (!captured){
                export(prenderer);
                captured = true;
            }
        } else {
            captured = false;
        }
        shift_pressed = kbstate[SDL_SCANCODE_LSHIFT];

        //Projecting
        if (do_hidden){
            do_hidden = false;
            update_transform_matrix(cam.transform_mat, rotation, translation, orbit, orbit_radius);
            pculled_tri = project_tri_mesh(pbuffer, pscene, &cam);
            update_texture(ppixels, pbuffer, ptexture, pculled_tri, false, &cam);
            free(pculled_tri);
        } else if (reproject) {
            update_transform_matrix(cam.transform_mat, rotation, translation, orbit, orbit_radius);
            pculled_tri = project_tri_mesh(pbuffer, pscene, &cam);
            update_texture(ppixels, pbuffer, ptexture, pculled_tri, true, &cam);
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
