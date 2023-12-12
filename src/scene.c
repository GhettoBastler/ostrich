#include <stdlib.h>
#include <time.h>
#include "transforms.h"
#include "primitives.h"

#define N_PRISMS 100 
#define SPACING 50

Mesh3D* make_scene(){
    // Create a new mesh for the scene
    Mesh3D* pscene = (Mesh3D*) malloc(sizeof(Mesh3D));
    pscene->size = 0;
    if (pscene == NULL){
        fprintf(stderr, "Couldn't allocate memory when creating the scene\n");
        exit(1);
    }

    srandom(time(NULL));
    // Adding elements

    Mesh3D* ppolygon;
    Point3D extrude_vector = {0, 0, 50};
    Point3D rotate_vector = {0, 0, 0};
    Point3D translate_vector = {0, 0, 0};


    // A BUNCH OF PRISMS
    for (int i = 0; i < N_PRISMS; i++){
        ppolygon = polygon(10, 4 + (random() % 8));
        extrude_vector.z += random() % 100;
        Mesh3D* pnew_prism = prism(ppolygon, extrude_vector);
        extrude_vector.z = 20;

        rotate_vector.x = 2 * M_PI * (float)random() / (float)RAND_MAX;
        rotate_vector.y = 2 * M_PI * (float)random() / (float)RAND_MAX;
        rotate_vector.z = 2 * M_PI * (float)random() / (float)RAND_MAX;

        rotate(pnew_prism, rotate_vector);

        translate_vector.x = SPACING * ((random() % 10) - 5);
        translate_vector.y = SPACING * ((random() % 10) - 5);
        translate_vector.z = SPACING * ((random() % 10) - 5);

        translate(pnew_prism, translate_vector);
        pscene = merge_meshes(pscene, pnew_prism);
    }
    
    return pscene;
}

TriangleMesh* tri_make_scene(){
    int offset_x = 0, offset_z = 0;
    TriangleMesh* pres = tri_cube(10);
    TriangleMesh* curr_cube;
    for (int i = 0; i < 2; i++){
        offset_x = 0;
        for (int j = 0; j < 2; j++){
            curr_cube = tri_cube(10);
            for (int k = 0; k < curr_cube->size; k++){
                curr_cube->triangles[k].a.x += offset_x;
                curr_cube->triangles[k].b.x += offset_x;
                curr_cube->triangles[k].c.x += offset_x;

                curr_cube->triangles[k].a.z += offset_z;
                curr_cube->triangles[k].b.z += offset_z;
                curr_cube->triangles[k].c.z += offset_z;
            }
            pres = merge_tri_meshes(pres, curr_cube);
            offset_x += 20;
        }
        offset_z += 20;
    }
    return pres;
}
