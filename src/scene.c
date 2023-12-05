#include <stdlib.h>
#include "transforms.h"
#include "primitives.h"

Mesh3D* make_scene(){
    Mesh3D* pscene = (Mesh3D*) malloc(sizeof(Mesh3D));
    pscene->size = 0;
    if (pscene == NULL){
        fprintf(stderr, "Couldn't allocate memory when creating the scene\n");
        exit(1);
    }
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
    //Mesh3D* pgrid = grid(1000, 1000, 20, 20);
    //pscene = merge_meshes(pscene, pgrid);
    pscene = merge_meshes(pscene, psphere);
    pscene = merge_meshes(pscene, pbox2);
    pscene = merge_meshes(pscene, pprism);
    
    return pscene;
}
