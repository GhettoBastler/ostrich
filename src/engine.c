#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600
#define FPS 60
#define CIRCLE_SEGMENTS 15
#define SPHERE_SLICES 15
#define EXPORT_PATH "export.bmp"

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

Mesh3D* grid(float width, float length, int n_div_x, int n_div_y){
    Mesh3D* pres = (Mesh3D*) malloc(sizeof(Mesh3D) + (4 + n_div_x + n_div_y) * sizeof(Edge3D));
    Point3D a = {-width/2, -length/2, 0};
    Point3D b = {width/2, -length/2, 0};
    Point3D c = {width/2, length/2, 0};
    Point3D d = {-width/2, length/2, 0};
    Edge3D ab = {a, b};
    Edge3D bc = {b, c};
    Edge3D cd = {c, d};
    Edge3D da = {d, a};
    pres->edges[0] = ab;
    pres->edges[1] = bc;
    pres->edges[2] = cd;
    pres->edges[3] = da;
    pres->size = 4;
    Edge3D curr_edge;
    Point3D curr_a, curr_b;
    curr_a.z = curr_b.z = 0;
    for (int i=1; i<=n_div_x; i++){
        curr_a.x = curr_b.x = -width/2 + i*width/(1+n_div_x);
        curr_a.y = -length/2;
        curr_b.y = length/2;
        curr_edge.a = curr_a;
        curr_edge.b = curr_b;
        pres->edges[pres->size] = curr_edge;
        pres->size += 1;
    }
    for (int i=1; i<=n_div_y; i++){
        curr_a.y = curr_b.y = -width/2 + i*width/(1+n_div_y);
        curr_a.x = -length/2;
        curr_b.x = length/2;
        curr_edge.a = curr_a;
        curr_edge.b = curr_b;
        pres->edges[pres->size] = curr_edge;
        pres->size += 1;
    }
    return pres;
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
    // http://members.chello.at/~easyfilter/bresenham.html
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
            ppixels[x0 + WIDTH * y0] = 0xFFFFFFFF;

        if (x0 == x1 && y0 == y1) break;
        e2 = 2*err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw(Uint32* ppixels, Mesh2D* pmesh, SDL_Texture* ptexture, SDL_Renderer* prenderer){
    int pitch = WIDTH * sizeof(Uint32);
    SDL_LockTexture(ptexture, NULL, (void**) &ppixels, &pitch);
    //Clear pixels
    for (int i = 0; i < HEIGHT * WIDTH; i++){
        ppixels[i] = 0x00000000;
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

    ptexture = SDL_CreateTexture(prenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

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
    Mesh3D* pscene = (Mesh3D*) malloc(sizeof(Mesh3D));
    pscene->size = 0;
    if (pscene == NULL){
        fprintf(stderr, "Couldn't allocate memory when creating the scene\n");
        return 1;
    }

    // Initializing camera
    Camera cam;
    cam.translation.x = 0;
    cam.translation.y = 0;
    cam.translation.z = 0;
    cam.rotation.x = 0;
    cam.rotation.y = 0;
    cam.rotation.z = 0;
    cam.focal_length = 800;

    // --- MODIFY HERE ---
    Mesh3D* psphere = sphere(10);
    Point3D v = {30, 20, 0};
    translate(psphere, v);
    Mesh3D* pbox2 = box(30, 30, 30);
    v.x = M_PI/4;
    v.y = M_PI/4;
    v.z = 0;
    rotate(pbox2, v);
    v.x = 20;
    v.y = 20;
    v.z = -30;
    translate(pbox2, v);
    Mesh3D* ppentagon = polygon(15, 5);
    v.x = 0;
    v.y = 0;
    v.z = 50;
    Mesh3D* pprism = prism(ppentagon, v);
    Mesh3D* pgrid = grid(1000, 1000, 20, 20);
    pscene = merge_meshes(pscene, pgrid);
    //pscene = merge_meshes(pscene, psphere);
    //pscene = merge_meshes(pscene, pbox2);
    pscene = merge_meshes(pscene, pprism);


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
