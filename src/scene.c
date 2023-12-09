#include <stdlib.h>
#include "transforms.h"
#include "primitives.h"

#define SIZE 4
#define SPACING 40

Mesh3D* make_scene(){
    // Create a new mesh for the scene
    Mesh3D* pscene = (Mesh3D*) malloc(sizeof(Mesh3D));
    pscene->size = 0;
    if (pscene == NULL){
        fprintf(stderr, "Couldn't allocate memory when creating the scene\n");
        exit(1);
    }
    // Adding elements

    // A BUNCH OF SPHERES
    Mesh3D* spheres[(int)pow(SIZE, 3)];
    for (int i = 0; i < pow(SIZE, 3); i++){
        Mesh3D* pnew_sphere = sphere(10);
        int a = i % SIZE,
            b = floor((i % (int)pow(SIZE, 2)) / SIZE),
            c = floor(i / pow(SIZE, 2));
        Point3D vector = {SPACING * a, SPACING * b, SPACING * c};
        translate(pnew_sphere, vector);
        pscene = merge_meshes(pscene, pnew_sphere);
    }
    
    return pscene;
}
