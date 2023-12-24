#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"
#include "camera.h"

TriangleMesh* add_triangle(TriangleMesh* pmesh, Triangle tri){
    //Allocating more memory to add the new triangle
    TriangleMesh* pres = realloc(pmesh, sizeof(TriangleMesh) + (pmesh->size + 1) * sizeof(Triangle));
    if (pres == NULL){
        fprintf(stderr, "Couldn't allocate memory to add a new triangle\n");
        exit(1);
    }
    pres->triangles[pres->size] = tri;
    pres->size += 1;
    return pres;
}

TriangleMesh* merge_tri_meshes(TriangleMesh* pmesh1, TriangleMesh* pmesh2){
    for (int i = 0; i < pmesh2->size; i++){
        pmesh1 = add_triangle(pmesh1, pmesh2->triangles[i]);
    }
    free(pmesh2);
    return pmesh1;
}

void flip_triangle(Triangle* ptri){
    Point3D tmp = ptri->a;
    bool tmp_vis;
    ptri->a = ptri->b;
    ptri->b = tmp;
    tmp_vis = ptri->visible[1];
    ptri->visible[1] = ptri->visible[2];
    ptri->visible[2] = tmp_vis;
}

void flip_mesh(TriangleMesh* pmesh){
    for (int i = 0; i < pmesh->size; i++){
        flip_triangle(&(pmesh->triangles[i]));
    }
}

TriangleMesh* extrude(Polygon* ppoly, float height){
    // Top
    TriangleMesh* poly1 = triangulate(ppoly);
    for (int i = 0; i < poly1->size; i++){
        poly1->triangles[i].a.z += height;
        poly1->triangles[i].b.z += height;
        poly1->triangles[i].c.z += height;
    }
    // Bottom
    TriangleMesh* poly2 = triangulate(ppoly);
    flip_mesh(poly2);

    // Sides
    TriangleMesh* psides = malloc(sizeof(TriangleMesh) + (2 * ppoly->size) * sizeof(Triangle));
    psides->size = 0;
    PolygonVertex* pcurr_vertex = ppoly->head;
    // Two triangles per sides
    Triangle tri1, tri2;
    do {
        // Triangle 1
        tri1.a.x = pcurr_vertex->next->coordinates.x;
        tri1.a.y = pcurr_vertex->next->coordinates.y;
        tri1.a.z = 0;
        tri1.b.x = pcurr_vertex->coordinates.x;
        tri1.b.y = pcurr_vertex->coordinates.y;
        tri1.b.z = 0;
        tri1.c.x = pcurr_vertex->coordinates.x;
        tri1.c.y = pcurr_vertex->coordinates.y;
        tri1.c.z = height;
        tri1.visible[0] = true;
        tri1.visible[1] = true;
        tri1.visible[2] = false;
        psides = add_triangle(psides, tri1);

        // Triangle 2
        tri2.a.x = pcurr_vertex->next->coordinates.x;
        tri2.a.y = pcurr_vertex->next->coordinates.y;
        tri2.a.z = 0;
        tri2.b.x = pcurr_vertex->coordinates.x;
        tri2.b.y = pcurr_vertex->coordinates.y;
        tri2.b.z = height;
        tri2.c.x = pcurr_vertex->next->coordinates.x;
        tri2.c.y = pcurr_vertex->next->coordinates.y;
        tri2.c.z = height;
        tri2.visible[0] = false;
        tri2.visible[1] = true;
        tri2.visible[2] = true;
        psides = add_triangle(psides, tri2);

        pcurr_vertex = pcurr_vertex->next;
    } while (pcurr_vertex != ppoly->head);

    // Merging
    poly1 = merge_tri_meshes(poly1, poly2);
    poly1 = merge_tri_meshes(poly1, psides);
    return poly1;
}
