#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"

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
