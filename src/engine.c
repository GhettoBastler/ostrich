#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600
#define FPS 60
#define CIRCLE_SEGMENTS 20
#define SPHERE_SLICES 20

// Structures
// 2D
typedef struct {
    float x, y;
} Point2D;

typedef struct {
    Point2D a, b;
} Edge2D;

typedef struct {
    int size;
    Edge2D edges[];
} Mesh2D;

// 3D
typedef struct {
   float x, y, z; 
} Point3D;

typedef struct {
    Point3D a, b;
} Edge3D;

typedef struct {
    int size;
    Edge3D edges[];
} Mesh3D;

// Functions
// Mesh transformations
Mesh3D* add_edge(Mesh3D* pmesh, Edge3D edge){
    //Allocating more memory to add the new edge
    Mesh3D* pres = realloc(pmesh, sizeof(Mesh3D) + (pmesh->size + 1) * sizeof(Edge3D));
    if (pres == NULL){
        fprintf(stderr, "Couldn't allocate memory to add a new edge\n");
        exit(1);
    }
    pres->edges[pres->size] = edge;
    pres->size += 1;
    return pres;
}

Mesh3D* merge_meshes(Mesh3D* pmesh1, Mesh3D* pmesh2){
    for (int i = 0; i < pmesh2->size; i++){
        pmesh1 = add_edge(pmesh1, pmesh2->edges[i]);
    }
    free(pmesh2);
    return pmesh1;
}

void rotate(Mesh3D* pmesh, Point3D r){
    float c00 = cosf(r.x) * cosf(r.y);
    float c01 = cosf(r.x) * sinf(r.y) * sinf(r.z) - sinf(r.x) * cosf(r.z);
    float c02 = cosf(r.x) * sinf(r.y) * cosf(r.z) + sinf(r.x) * sinf(r.z);
    float c10 = sinf(r.x) * cosf(r.y);
    float c11 = sinf(r.x) * sinf(r.y) * sinf(r.z) + cosf(r.x) * cosf(r.z);
    float c12 = sinf(r.x) * sinf(r.y) * cosf(r.z) - cosf(r.x) * sinf(r.z);
    float c20 = -sinf(r.y);
    float c21 = cosf(r.y) * sinf(r.z);
    float c22 = cosf(r.y) * cosf(r.z);

    for (int i = 0; i < pmesh->size; i++){
        float xa = pmesh->edges[i].a.x;
        float ya = pmesh->edges[i].a.y;
        float za = pmesh->edges[i].a.z;
        float xb = pmesh->edges[i].b.x;
        float yb = pmesh->edges[i].b.y;
        float zb = pmesh->edges[i].b.z;

        pmesh->edges[i].a.x = c00 * xa + c01 * ya + c02 * za;
        pmesh->edges[i].a.y = c10 * xa + c11 * ya + c12 * za;
        pmesh->edges[i].a.z = c20 * xa + c21 * ya + c22 * za;
        pmesh->edges[i].b.x = c00 * xb + c01 * yb + c02 * zb;
        pmesh->edges[i].b.y = c10 * xb + c11 * yb + c12 * zb;
        pmesh->edges[i].b.z = c20 * xb + c21 * yb + c22 * zb;
    };
}

void translate(Mesh3D* pmesh, Point3D v){
    for (int i = 0; i < pmesh->size; i++){
        pmesh->edges[i].a.x += v.x;
        pmesh->edges[i].a.y += v.y;
        pmesh->edges[i].a.z += v.z;
        pmesh->edges[i].b.x += v.x;
        pmesh->edges[i].b.y += v.y;
        pmesh->edges[i].b.z += v.z;
    }
}

Mesh3D* prism(Mesh3D* pmesh, Point3D vect){
    Mesh3D* pcapA = (Mesh3D*) malloc(sizeof(Mesh3D) + pmesh->size * sizeof(Edge3D));
    Mesh3D* pcapB = (Mesh3D*) malloc(sizeof(Mesh3D) + pmesh->size * sizeof(Edge3D));
    if (pcapA == NULL || pcapB == NULL){
        fprintf(stderr, "Couldn't allocate memory for creating a prism\n");
        exit(1);
    }
    memcpy(pcapA, pmesh, sizeof(Mesh3D) + pmesh->size * sizeof(Edge3D));
    memcpy(pcapB, pmesh, sizeof(Mesh3D) + pmesh->size * sizeof(Edge3D));
    translate(pcapB, vect);
    Edge3D edge;
    for (int i = 0; i < pmesh->size; i++){
        edge.a = pcapA->edges[i].a;
        edge.b = pcapB->edges[i].a;
        pcapA = add_edge(pcapA, edge);
    }
    pcapA = merge_meshes(pcapA, pcapB);
    return pcapA;
}

// 3D primitives
Mesh3D* box(float a, float b, float c){
    Mesh3D* pres = (Mesh3D*) malloc(sizeof(Mesh3D) + 12 * sizeof(Edge3D));

    if (pres == NULL){
        fprintf(stderr, "Couldn't allocate memory for creating a box\n");
        exit(1);
    }

    Point3D ptA = {-a/2, -b/2, -c/2};
    Point3D ptB = {a/2, -b/2, -c/2};
    Point3D ptC = {a/2, b/2, -c/2};
    Point3D ptD = {-a/2, b/2, -c/2};
    Point3D ptE = {-a/2, -b/2, c/2};
    Point3D ptF = {a/2, -b/2, c/2};
    Point3D ptG = {a/2, b/2, c/2};
    Point3D ptH = {-a/2, b/2, c/2};
    Edge3D edgeAB = {ptA, ptB};
    Edge3D edgeBC = {ptB, ptC};
    Edge3D edgeCD = {ptC, ptD};
    Edge3D edgeDA = {ptD, ptA};
    Edge3D edgeEF = {ptE, ptF};
    Edge3D edgeFG = {ptF, ptG};
    Edge3D edgeGH = {ptG, ptH};
    Edge3D edgeHE = {ptH, ptE};
    Edge3D edgeAE = {ptA, ptE};
    Edge3D edgeBF = {ptB, ptF};
    Edge3D edgeCG = {ptC, ptG};
    Edge3D edgeDH = {ptD, ptH};

    pres->edges[0] = edgeAB;
    pres->edges[1] = edgeBC;
    pres->edges[2] = edgeCD;
    pres->edges[3] = edgeDA;
    pres->edges[4] = edgeEF;
    pres->edges[5] = edgeFG;
    pres->edges[6] = edgeGH;
    pres->edges[7] = edgeHE;
    pres->edges[8] = edgeAE;
    pres->edges[9] = edgeBF;
    pres->edges[10] = edgeCG;
    pres->edges[11] = edgeDH;

    pres->size = 12;

    return pres;
}

Mesh3D* polygon(float radius, int n){
    Mesh3D* pmesh = malloc(sizeof(Mesh3D) + n * sizeof(Edge3D));
    if (pmesh == NULL) {
        fprintf(stderr, "Couldn't allocate memory to create polygon\n");
        exit(1);
    }
    pmesh->size = n;
    Point3D curr_pt = {0, 0, 0}, prev_pt = {radius, 0, 0};
    Edge3D edge;

    for (int i = 1; i < n; i++){
        float x = radius * cosf(i * M_PI * 2 / n);
        float y = radius * sinf(i * M_PI * 2 / n);
        curr_pt.x = x;
        curr_pt.y = y;
        edge.a = prev_pt;
        edge.b = curr_pt;
        pmesh->edges[i-1] = edge;
        prev_pt = curr_pt;
    }
    edge.a = prev_pt;
    edge.b = pmesh->edges[0].a;
    pmesh->edges[n-1] = edge;
    return pmesh;
}

Mesh3D* line(float xa, float ya, float za, float xb, float yb, float zb){
    Mesh3D* pmesh = malloc(sizeof(Mesh3D) + sizeof(Edge3D));
    if (pmesh == NULL) {
        fprintf(stderr, "Couldn't allocate memory to create a line\n");
        exit(1);
    }
    pmesh->size = 1;
    Point3D ptA = {xa, ya, za};
    Point3D ptB = {xb, yb, zb};
    Edge3D edge = {ptA, ptB};
    pmesh->edges[0] = edge;
    return pmesh;
}

Mesh3D* sphere(float radius){
    Mesh3D* pres = malloc(sizeof(Mesh3D));
    if (pres == NULL) {
        fprintf(stderr, "Couldn't allocate memory to create a sphere\n");
        exit(1);
    }
    pres->size = 0;
    Point3D v = {0, 0, 0};
    for (int i = 1; i <= SPHERE_SLICES; i++){
        Mesh3D* pcircle = polygon(radius * sinf(i * M_PI / (1 + SPHERE_SLICES)), CIRCLE_SEGMENTS);
        v.z = radius * cosf(i * M_PI / (1 + SPHERE_SLICES));
        translate(pcircle, v);
        pres = merge_meshes(pres, pcircle); // this frees pcircle
        // Stitching
        if (i > 1){
            Edge3D edges[CIRCLE_SEGMENTS];
            Edge3D curr_edge;
            for (int j = 0; j < CIRCLE_SEGMENTS; j++){
                curr_edge.a = pres->edges[pres->size - 1 - j].a;
                curr_edge.b = pres->edges[pres->size - 1 - j - CIRCLE_SEGMENTS].a;
                edges[j] = curr_edge;
            }
            for (int j = CIRCLE_SEGMENTS - 1; j >= 0; j--){
                pres = add_edge(pres, edges[j]);
            }
        }
    }
    //Caps
    Mesh3D* pcaptop = malloc(sizeof(Mesh3D));
    Mesh3D* pcapbottom = malloc(sizeof(Mesh3D));
    if (pcaptop == NULL || pcapbottom == NULL) {
        fprintf(stderr, "Couldn't allocate memory to create the sphere caps\n");
        exit(1);
    }
    pcaptop->size = pcapbottom->size = 0;
    Point3D ptTop = {0, 0, radius};
    Point3D ptBottom = {0, 0, -radius};
    Edge3D edge;
    for (int i = 0; i < CIRCLE_SEGMENTS; i++){
        //Top cap
        edge.a = ptTop;
        edge.b = pres->edges[i].a;
        pcaptop = add_edge(pcaptop, edge);
        //Bottom cap
        edge.a = ptBottom;
        edge.b = pres->edges[pres->size - CIRCLE_SEGMENTS*2 + i].a;
        pcapbottom = add_edge(pcapbottom, edge);
    }
    pres = merge_meshes(pres, pcaptop);
    pres = merge_meshes(pres, pcapbottom);
    return pres;
}

// 2D projection
Point2D project_point(Point3D point, float dist, float focal_length){
    float x = (point.x * (focal_length / (dist + point.z))) + (WIDTH / 2);
    float y = (point.y * (focal_length / (dist + point.z))) + (HEIGHT / 2);
    Point2D res = {x, y};
    return res;
}

Edge2D project_edge(Edge3D edge, float dist, float focal_length){
    Point2D a = project_point(edge.a, dist, focal_length);
    Point2D b = project_point(edge.b, dist, focal_length);
    Edge2D res = {a, b};
    return res;
}

void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, float dist, float focal_length){
    for (int i = 0; i < pmesh->size; i++){
        pbuffer->edges[i] = project_edge(pmesh->edges[i], dist, focal_length);
    }
    pbuffer->size = pmesh->size;
}

void draw(Mesh2D* pmesh, SDL_Renderer* prenderer){
    //Clear screen
    SDL_SetRenderDrawColor(prenderer, 10, 10, 10, 255);
    SDL_RenderClear(prenderer);
    //Draw 
    SDL_SetRenderDrawColor(prenderer, 255, 100, 100, 255);
    for (int i = 0; i < pmesh->size; i++){
        SDL_RenderDrawLine(prenderer,
            (int)pmesh->edges[i].a.x,
            (int)pmesh->edges[i].a.y,
            (int)pmesh->edges[i].b.x,
            (int)pmesh->edges[i].b.y);
    }
    SDL_RenderPresent(prenderer);
}

// Test functions (to remove)
void print_mesh(Mesh3D* pmesh){
    printf("%i edges\n", pmesh->size);
    for (int i = 0; i < pmesh->size; i++){
        printf("%i: (%.1f,%.1f,%.1f) to (%.1f,%.1f,%.1f)\n",
                i,
                pmesh->edges[i].a.x,
                pmesh->edges[i].a.y,
                pmesh->edges[i].a.z,
                pmesh->edges[i].b.x,
                pmesh->edges[i].b.y,
                pmesh->edges[i].b.z);
    }
    printf("\n");
}

int main(int argc, char **argv){
    // SDL Initialization
    SDL_Window* pwindow = NULL;
    SDL_Renderer* prenderer = NULL;

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

    // Initializing main loop
    // Creating scene
    Mesh3D* pscene = (Mesh3D*) malloc(sizeof(Mesh3D));
    if (pscene == NULL){
        fprintf(stderr, "Couldn't allocate memory when creating the scene\n");
        return 1;
    }

    // --- MODIFY HERE ---
    pscene = sphere(30);

    // Creating a buffer for the 2D projection
    Mesh2D* pbuffer = (Mesh2D*) malloc(sizeof(Mesh2D) + pscene->size * sizeof(Edge2D));
    if (pbuffer == NULL){
        fprintf(stderr, "Couldn't create the buffer for the 2D projection\n");
        return 1;
    }

    // Main loop

    Uint32 time_start, delta;
    SDL_Event event;

    bool is_stopped = false;
    bool button_pressed = false;
    bool shift_pressed = false;
    int prev_x, prev_y;

    float zoom = 100.;
    Point3D rotation, translation;


    while (!is_stopped){
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
                            translation.x = (float)(event.motion.x - prev_x)/5;
                            translation.y = (float)(event.motion.y - prev_y)/5;
                            translation.z = 0;
                            translate(pscene, translation);
                        } else {
                            rotation.x = 0;
                            rotation.y = -(float)(event.motion.x - prev_x)/100;
                            rotation.z = (float)(event.motion.y - prev_y)/100;
                            rotate(pscene, rotation);
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
                    break;
            }
        }

        //Drawing
        project_mesh(pbuffer, pscene, zoom, 800);
        draw(pbuffer, prenderer);

        //FPS caping
        delta = SDL_GetTicks() - time_start;
        if (delta == 0 || 1000 / delta > FPS) {
            SDL_Delay((1000 / FPS) - delta);
        }
    }

    // Freeing
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;
}
