#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "primitives.h"

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
