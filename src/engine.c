#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "transforms.h"
#include "primitives.h"
#include "scene.h"

#define WIDTH 800
#define HEIGHT 600
#define FPS 60
#define EXPORT_PATH "export.bmp"

// Structures
// Camera
typedef struct {
    Point3D translation, rotation;
    float focal_length;
    float rot_mat[9];
} Camera;

// Functions
// Point transformation
Point3D rotate_point(Point3D point, Camera* pcam){
    Point3D res;

    res.x = pcam->rot_mat[0] * point.x + pcam->rot_mat[1] * point.y + pcam->rot_mat[2] * point.z;
    res.y = pcam->rot_mat[3] * point.x + pcam->rot_mat[4] * point.y + pcam->rot_mat[5] * point.z;
    res.z = pcam->rot_mat[6] * point.x + pcam->rot_mat[7] * point.y + pcam->rot_mat[8] * point.z;

    return res;
}

Point3D translate_point(Point3D point, Point3D vector){
    Point3D res;

    res.x = point.x + vector.x;
    res.y = point.y + vector.y;
    res.z = point.z + vector.z;

    return res;
}

// 2D projection
Point2D project_point(Point3D point, float dist, float focal_length){
    float x = (point.x * (focal_length / (point.z))) + (WIDTH / 2);
    float y = (point.y * (focal_length / (point.z))) + (HEIGHT / 2);
    Point2D res = {x, y};
    return res;
}

Edge2D project_edge(Edge3D edge, float dist, float focal_length){
    Point2D a = project_point(edge.a, dist, focal_length);
    Point2D b = project_point(edge.b, dist, focal_length);
    Edge2D res = {a, b};
    return res;
}

void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, Camera* pcam){
    int n = 0;
    for (int i = 0; i < pmesh->size; i++){
        Point3D a_trans, b_trans;
        Point3D a = rotate_point(pmesh->edges[i].a, pcam);
        a = translate_point(a, pcam->translation);
        Point3D b = rotate_point(pmesh->edges[i].b, pcam);
        b = translate_point(b, pcam->translation);

        // Check visibility
        bool a_hidden, b_hidden;
        a_hidden = a.z <= 0;
        b_hidden = b.z <= 0;

        if (a_hidden && b_hidden){
            // Both points are hidden
            // Skip this edge
            continue;
        } else if (a_hidden){
            // A is hidden, B is visible
            a.x = (a.z / (b.z - a.z)) * (b.x - a.x) - a.x;
            a.y = (a.z / (b.z - a.z)) * (b.y - a.y) - a.y;
            a.z = -1;
        } else if (b_hidden){
            // B is hidden, A is visible
            b.x = (b.z / (a.z - b.z)) * (a.x - b.x) - b.x;
            b.y = (b.z / (a.z - b.z)) * (a.y - b.y) - b.y;
            b.z = -1;
        } // If both are visible, we do nothing

        Edge3D new_edge = {a, b};
        pbuffer->edges[n] = project_edge(new_edge, pcam->translation.z, pcam->focal_length);
        n += 1;
    }
    pbuffer->size = n;
}

// Interface
void update_rotation_matrix(Camera* pcam){
    Point3D r = pcam->rotation;
    pcam->rot_mat[0] = cosf(r.x) * cosf(r.y);
    pcam->rot_mat[1] = cosf(r.x) * sinf(r.y) * sinf(r.z) - sinf(r.x) * cosf(r.z);
    pcam->rot_mat[2] = cosf(r.x) * sinf(r.y) * cosf(r.z) + sinf(r.x) * sinf(r.z);
    pcam->rot_mat[3] = sinf(r.x) * cosf(r.y);
    pcam->rot_mat[4] = sinf(r.x) * sinf(r.y) * sinf(r.z) + cosf(r.x) * cosf(r.z);
    pcam->rot_mat[5] = sinf(r.x) * sinf(r.y) * cosf(r.z) - cosf(r.x) * sinf(r.z);
    pcam->rot_mat[6] = -sinf(r.y);
    pcam->rot_mat[7] = cosf(r.y) * sinf(r.z);
    pcam->rot_mat[8] = cosf(r.y) * cosf(r.z);
}

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
    Camera cam;
    cam.translation.x = 0;
    cam.translation.y = 0;
    cam.translation.z = 0;
    cam.rotation.x = 0;
    cam.rotation.y = 0;
    cam.rotation.z = 0;
    cam.focal_length = 800;

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
    SDL_DestroyTexture(ptexture);
    SDL_DestroyRenderer(prenderer);
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;
}
