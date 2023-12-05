#include <stdlib.h>
#include "transforms.h"
#include "primitives.h"

Mesh3D* make_scene(){
    // Create a new mesh for the scene
    Mesh3D* pscene = (Mesh3D*) malloc(sizeof(Mesh3D));
    pscene->size = 0;
    if (pscene == NULL){
        fprintf(stderr, "Couldn't allocate memory when creating the scene\n");
        exit(1);
    }
    // Adding elements

    // A BUNCH OF CUBES
    Mesh3D* cubes[100];
    for (int i = 0; i < 100; i++){
        Mesh3D* pnew_cube = box(10, 10, 10);
        Point3D vector = {30 * (i % 10), 30 * floor(i / 10), 0};
        translate(pnew_cube, vector);
        pscene = merge_meshes(pscene, pnew_cube);
    }
    
    return pscene;
}
