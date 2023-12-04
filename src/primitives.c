#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "primitives.h"
#include "transforms.h"

#define CIRCLE_SEGMENTS 15
#define SPHERE_SLICES 15

// 3D MESHES
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
