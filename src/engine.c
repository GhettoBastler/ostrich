#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "transforms.h"
#include "primitives.h"
#include "interpreter.h"
#include "camera.h"
#include "vect.h"
#include "ui.h"
#include "engine.h"
#include "render.h"

#define KBSTATE_SIZE 256
#define FPS 60
#define EXPORT_PATH "export.bmp"


static EngineState engine_state = {false, false, false, false, true};

// Rendering
static SDL_Window* pwindow = NULL;
static SDL_Renderer* prenderer = NULL;
static SDL_Texture* ptexture = NULL;

// Model
static char* input_file_path;
static FILE* pfile = NULL;
static TriangleMesh* pscene = NULL;

// Camera
static Camera cam;
static Point3D rotation, translation;

// Keyboard
static Uint8 old_kbstate[KBSTATE_SIZE];
static const Uint8* kbstate;

// Mouse
static Uint32 mousestate;
static int prev_x, prev_y;
static int mouse_x, mouse_y;

// FPS capping
static Uint32 time_start;

// Event loop
static bool is_stopped = false;
static SDL_Event event;


void put_on_screen();
void export(SDL_Renderer* prenderer);
void load_scene();
void render(TriangleMesh* pmesh);
void init_rendering();
void process_keys();
void process_mouse();
void cap_fps();


int main(int argc, char **argv){
    // Read file path from argument
    input_file_path = argv[1];

    // Initializing
    init_rendering();
    init_ui(HEIGHT, WIDTH, prenderer);
    load_scene();

    kbstate = SDL_GetKeyboardState(NULL);
    cam = make_camera((float)WIDTH/SCALE, (float)HEIGHT/SCALE, 800/SCALE);
 
    // Main loop
    engine_state.reproject = true;

    while (!is_stopped){
        translation.x = translation.y = translation.z = 0;
        rotation.x = rotation.y = rotation.z = 0;
        time_start = SDL_GetTicks();
        
        //Processing inputs
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    is_stopped = true;
                    break;

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        cam.focal_length += 1;
                    } else {
                        cam.focal_length -= 1;
                        if (cam.focal_length <= 0)
                            cam.focal_length = 0.001;
                    }
                    engine_state.reproject = true;
                    break;
            }
        }
        process_mouse();
        process_keys();

        //Projecting
        if (engine_state.reproject){
            update_transform_matrix(cam.transform_mat, rotation, translation,
                                    engine_state.orbit, cam.orbit_radius);
            TriangleMesh* ptransformed = transform_and_cull(
                    pscene, &cam, engine_state.bface_cull);
            render(ptransformed);
            free(ptransformed);
            if (engine_state.do_hlr){
                engine_state.hlr = true;
                engine_state.do_hlr = false;
            } else {
                engine_state.hlr = false;
            }
            engine_state.reproject = false;
        }
        engine_state.reproject = false;

        //Drawing
        put_on_screen();

        //FPS caping
        cap_fps();
    }
    printf("Exiting...\n");

    // Freeing
    free(pscene);

    SDL_DestroyTexture(ptexture);
    SDL_DestroyRenderer(prenderer);
    SDL_DestroyWindow(pwindow); SDL_Quit();
    return 0;
}

void put_on_screen(){
    SDL_RenderCopy(prenderer, ptexture, NULL, NULL);
    draw_ui(prenderer, engine_state);
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

void load_scene(){
    if (pscene != NULL)
        free(pscene);
    // Open input file
    pfile = fopen(input_file_path, "r");

    if (pfile == NULL){
        printf("No such file\n");
        exit(1);
    }
    pscene = mesh_from_file(pfile);
}

void render(TriangleMesh* pmesh){
    int pitch = WIDTH * sizeof(Uint32);
    Uint32* ppixels = NULL;
    SDL_LockTexture(ptexture, NULL, (void**) &ppixels, &pitch);
    render_mesh(pmesh, ppixels, &cam, engine_state.do_hlr);
    SDL_UnlockTexture(ptexture);
}

void init_rendering(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
        exit(1);
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

    ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    check_allocation(ptexture, "SDL texture failed to initialize\n");
}

void process_keys(){
    if (kbstate[SDL_SCANCODE_W]) {
        // Move forward
        translation.z = -1;
        cam.orbit_radius += -1;
        engine_state.reproject = true;
    } else if (kbstate[SDL_SCANCODE_S]) {
        // Move backward
        translation.z = 1;
        cam.orbit_radius += 1;
        engine_state.reproject = true;
    }
    if (kbstate[SDL_SCANCODE_A]) {
        // Move left
        translation.x = 1;
        engine_state.reproject = true;
    } else if (kbstate[SDL_SCANCODE_D]) {
        // Move right
        translation.x = -1;
        engine_state.reproject = true;
    }
    if (kbstate[SDL_SCANCODE_Q]) {
        // Move up
        translation.y = 1;
        engine_state.reproject = true;
    } else if (kbstate[SDL_SCANCODE_E]) {
        // Move down
        translation.y = -1;
        engine_state.reproject = true;
    }
    if (kbstate[SDL_SCANCODE_O] && !old_kbstate[SDL_SCANCODE_O]) {
        // Toggle orbit mode
        engine_state.orbit = !engine_state.orbit;
        printf("Orbit mode toggled\n");
    }

    if (kbstate[SDL_SCANCODE_B] && !old_kbstate[SDL_SCANCODE_B]) {
        // Toggle back-face culling
        engine_state.bface_cull = !engine_state.bface_cull;
        engine_state.reproject = true;
    }

    if (kbstate[SDL_SCANCODE_R] && !old_kbstate[SDL_SCANCODE_R]) {
        // Trigger hidden-line removal
        engine_state.bface_cull = true;
        engine_state.do_hlr = true;
        engine_state.reproject = true;
    }

    if (kbstate[SDL_SCANCODE_SPACE] && !old_kbstate[SDL_SCANCODE_SPACE]) {
        // Trigger screenshot
        export(prenderer);
    }

    if (kbstate[SDL_SCANCODE_T] && !old_kbstate[SDL_SCANCODE_T]) {
        // Reload file
        printf("Reloading input file\n");
        load_scene();
        engine_state.reproject = true;
    }

    // Save the current state of the keyboard
    memcpy(old_kbstate, kbstate, KBSTATE_SIZE * sizeof(Uint8));
}

void process_mouse(){
    // Mouse
    mousestate = SDL_GetMouseState(&mouse_x, &mouse_y);
    // Dirty check to check in which part of the window is the mouse
    if (mouse_y < HEIGHT - BAR_HEIGHT) {
        // Top part
        if (mousestate & SDL_BUTTON(1)){
            if (kbstate[SDL_SCANCODE_LSHIFT]) {
                translation.x = (float)(mouse_x - prev_x)/2;
                translation.y = (float)(mouse_y - prev_y)/2;
            } else {
                rotation.y = -(float)(mouse_x - prev_x)/500;
                rotation.x = (float)(mouse_y - prev_y)/500;
            }
            engine_state.reproject = true;
        } else if (mousestate & SDL_BUTTON(3)){
            rotation.z = (float)(mouse_x - prev_x)/500;
            engine_state.reproject = true;
        }

        prev_x = mouse_x;
        prev_y = mouse_y;
    } else {
        // Bottom part
        process_ui_click(mouse_x, mouse_y, mousestate, &engine_state);
    }
}

void cap_fps(){
    Uint32 delta = SDL_GetTicks() - time_start;
    if (delta == 0 || 1000 / delta > FPS) {
        SDL_Delay((1000 / FPS) - delta);
    }
};
