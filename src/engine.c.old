#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define FOCAL_LENGTH 800
#define FPS 60.0

typedef struct {
    float x, y;
} point2D;

typedef struct {
    point2D a, b;
} edge2D;

typedef struct {
    int n;
    edge2D edges[1000];
} wireframe2D;

typedef struct {
    float x, y, z;
} point3D;

typedef struct {
    point3D a, b;
} edge3D;

typedef struct {
    int n;
    edge3D edges[1000];
} wireframe3D;

void addWf(wireframe3D *wf, wireframe3D *toAdd){
    for (int i = 0; i < toAdd->n; i++){
        wf->edges[wf->n + i] = toAdd->edges[i];
    }
    wf->n += toAdd->n;
};

void translate(wireframe3D *wf, point3D v){
    for (int i = 0; i < wf->n; i++){
        wf->edges[i].a.x += v.x;
        wf->edges[i].a.y += v.y;
        wf->edges[i].a.z += v.z;
        wf->edges[i].b.x += v.x;
        wf->edges[i].b.y += v.y;
        wf->edges[i].b.z += v.z;
    };
};

void rotate(wireframe3D *wf, point3D r){
    float c00 = cosf(r.x) * cosf(r.y);
    float c01 = cosf(r.x) * sinf(r.y) * sinf(r.z) - sinf(r.x) * cosf(r.z);
    float c02 = cosf(r.x) * sinf(r.y) * cosf(r.z) + sinf(r.x) * sinf(r.z);
    float c10 = sinf(r.x) * cosf(r.y);
    float c11 = sinf(r.x) * sinf(r.y) * sinf(r.z) + cosf(r.x) * cosf(r.z);
    float c12 = sinf(r.x) * sinf(r.y) * cosf(r.z) - cosf(r.x) * sinf(r.z);
    float c20 = -sinf(r.y);
    float c21 = cosf(r.y) * sinf(r.z);
    float c22 = cosf(r.y) * cosf(r.z);

    for (int i = 0; i < wf->n; i++){
        float xa = wf->edges[i].a.x;
        float ya = wf->edges[i].a.y;
        float za = wf->edges[i].a.z;
        float xb = wf->edges[i].b.x;
        float yb = wf->edges[i].b.y;
        float zb = wf->edges[i].b.z;

        wf->edges[i].a.x = c00 * xa + c01 * ya + c02 * za;
        wf->edges[i].a.y = c10 * xa + c11 * ya + c12 * za;
        wf->edges[i].a.z = c20 * xa + c21 * ya + c22 * za;
        wf->edges[i].b.x = c00 * xb + c01 * yb + c02 * zb;
        wf->edges[i].b.y = c10 * xb + c11 * yb + c12 * zb;
        wf->edges[i].b.z = c20 * xb + c21 * yb + c22 * zb;
    };
};

wireframe3D line(float xa, float ya, float xb, float yb){
    point3D ptA = {xa, ya};
    point3D ptB = {xb, yb};
    edge3D e = {ptA, ptB};
    wireframe3D res;
    res.n = 1;
    res.edges[0] = e;
    return res;
}

wireframe3D cube(float a, float b, float c){
    point3D ptA = {-a/2, -b/2, -c/2};
    point3D ptB = {a/2, -b/2, -c/2};
    point3D ptC = {a/2, b/2, -c/2};
    point3D ptD = {-a/2, b/2, -c/2};
    point3D ptE = {-a/2, -b/2, c/2};
    point3D ptF = {a/2, -b/2, c/2};
    point3D ptG = {a/2, b/2, c/2};
    point3D ptH = {-a/2, b/2, c/2};
    edge3D ab = {ptA, ptB};
    edge3D bc = {ptB, ptC};
    edge3D cd = {ptC, ptD};
    edge3D da = {ptD, ptA};
    edge3D ef = {ptE, ptF};
    edge3D fg = {ptF, ptG};
    edge3D gh = {ptG, ptH};
    edge3D he = {ptH, ptE};
    edge3D ae = {ptA, ptE};
    edge3D bf = {ptB, ptF};
    edge3D cg = {ptC, ptG};
    edge3D dh = {ptD, ptH};

    wireframe3D res = {12, {ab, bc, cd, da, ae, bf, cg, dh, ef, fg, gh, he}};
    return res;
};

wireframe3D polygon(float radius, int n){
    wireframe3D res;
    res.n = n;
    point3D curr_pt = {0, 0, 0}, prev_pt = {radius, 0, 0};
    edge3D edge;

    for (int i = 1; i < n; i++){
        float x = radius * cosf(i * M_PI * 2 / n);
        float y = radius * sinf(i * M_PI * 2 / n);
        curr_pt.x = x;
        curr_pt.y = y;
        edge.a = prev_pt;
        edge.b = curr_pt;
        res.edges[i-1] = edge;
        prev_pt = curr_pt;
    }
    edge.a = prev_pt;
    edge.b = res.edges[0].a;
    res.edges[n-1] = edge;
    return res;
}

void extrude(wireframe3D* pwf, point3D vect){
    wireframe3D cap;
    memcpy(&cap, pwf, sizeof(*pwf));
    translate(&cap, vect);
    edge3D curr_edge;
    for (int i = 0; i < cap.n; i++){
        curr_edge.a = pwf->edges[i].a;
        curr_edge.b = cap.edges[i].a;
        pwf->edges[pwf->n] = curr_edge;
        pwf->n += 1;
    }
    addWf(pwf, &cap);
};

point2D project_point(point3D pt, float zoom){
    float x = (pt.x * (FOCAL_LENGTH / (zoom + pt.z))) + (WIDTH / 2);
    float y = (pt.y * (FOCAL_LENGTH / (zoom + pt.z))) + (HEIGHT / 2);
    point2D res = {x, y};
    return res;
};

edge2D project_edge(edge3D edg, float zoom){
    point2D a = project_point(edg.a, zoom);
    point2D b = project_point(edg.b, zoom);
    edge2D res = {a, b};
    return res;
};

wireframe2D project_wireframe(wireframe3D wf, float zoom){
    edge2D edges[100];
    for (int i = 0; i < wf.n; i++){
        edges[i] = project_edge(wf.edges[i], zoom);
    }
    wireframe2D res;
    res.n= wf.n;
    memcpy(res.edges, edges, sizeof(res.edges));
    return res;
};

void draw_wf2D(SDL_Renderer *renderer, wireframe2D wf){
    for (int i = 0; i < wf.n; i++){
        SDL_RenderDrawLine(renderer,
            (int)wf.edges[i].a.x,
            (int)wf.edges[i].a.y,
            (int)wf.edges[i].b.x,
            (int)wf.edges[i].b.y
        );
    }
};

int main (int argc, char **argv)
{
    //Initializing window
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "SDL failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL Example",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WIDTH,
            HEIGHT,
            0);

    if (window == NULL) {
        fprintf(stderr, "SDL window failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    //Creating renderer
    SDL_Renderer *renderer = NULL;

    renderer = SDL_CreateRenderer(window,
            -1,
            0);

    if (renderer == NULL) {
        fprintf(stderr, "SDL renderer failed to initialize: %s\n", SDL_GetError());
        return 1;
    }

    //Main loop
    Uint32 time_start, delta;
    bool is_stopped = false;
    bool button_pressed = false;
    bool shift_pressed = false;
    int prev_x, prev_y;
    float zoom = 100.;
    point3D vect;

    wireframe3D scene;

    // Line
    //wireframe3D randomline = line(0, 10, 50, -10);
    //addWf(&scene, &randomline);

    // Prisms
    point3D vect_ext;
    wireframe3D poly1 = polygon(10, 6);
    vect_ext.x = 0;
    vect_ext.y = 0;
    vect_ext.z = 50;
    extrude(&poly1, vect_ext);

    wireframe3D poly2 = polygon(10, 6);
    vect_ext.z = 30;
    extrude(&poly2, vect_ext);

    vect_ext.x = 10 + 10 * sinf(M_PI / 6);
    vect_ext.y = 10 * cosf(M_PI / 6);
    vect_ext.z = 0;
    translate(&poly2, vect_ext);

    wireframe3D poly3 = polygon(10, 6);
    vect_ext.x = 0;
    vect_ext.y = 0;
    vect_ext.z = 20;
    extrude(&poly3, vect_ext);
    vect_ext.x = 10 + 10 * sinf(M_PI / 6);
    vect_ext.y = -10 * cosf(M_PI / 6);
    vect_ext.z = 0;
    translate(&poly3, vect_ext);

    addWf(&scene, &poly1);
    addWf(&scene, &poly2);
    addWf(&scene, &poly3);

    wireframe2D projection;

    while (!is_stopped)
    {
        time_start = SDL_GetTicks();
        //Event processing
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
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
                            vect.x = (float)(event.motion.x - prev_x)/5;
                            vect.y = (float)(event.motion.y - prev_y)/5;
                            vect.z = 0;
                            translate(&scene, vect);
                        } else {
                            vect.x = 0;
                            vect.y = -(float)(event.motion.x - prev_x)/100;
                            vect.z = (float)(event.motion.y - prev_y)/100;
                            rotate(&scene, vect); 
                        }
                        prev_x = event.motion.x;
                        prev_y = event.motion.y;
                    }
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LSHIFT) {
                        shift_pressed = event.key.state == SDL_PRESSED;
                    }
                    break;
                
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        zoom += 1;
                    } else {
                        zoom -= 1;
                }
            }
        }

        //Drawing
        //Background
        SDL_SetRenderDrawColor(renderer, 
                10,
                10,
                10,
                255);
        SDL_RenderClear(renderer);

        //Line
        SDL_SetRenderDrawColor(renderer,
                255,
                100,
                100,
                255);

        projection = project_wireframe(scene, zoom);
        draw_wf2D(renderer, projection);

        //Displaying
        SDL_RenderPresent(renderer);

        //FPS
        delta = SDL_GetTicks() - time_start;
        if (delta == 0 || 1000 / delta < FPS) {
            SDL_Delay((1000 / FPS) - delta);
        }
    }

    //Freeing
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};
