#include <stdlib.h>
#include <time.h>
#include "vect.h"
#include "transforms.h"
#include "primitives.h"
#include "stl.h"

#define STL_FILE "input.stl"
#define SCALE 1

TriangleMesh* tri_make_scene(){
    // FILE* pfile = fopen(STL_FILE, "r");
    // if (pfile == NULL){
    //     printf("No such file\n");
    //     exit(1);
    // }
    // TriangleMesh* pscene = stl_to_tri_mesh(pfile);

    // // Scale model
    // for (int i = 0; i < pscene->size; i++){
    //     pscene->triangles[i].a = pt_mul(SCALE, pscene->triangles[i].a);
    //     pscene->triangles[i].b = pt_mul(SCALE, pscene->triangles[i].b);
    //     pscene->triangles[i].c = pt_mul(SCALE, pscene->triangles[i].c);
    // }

    // fclose(pfile);
    Point2D vertices[6] = {
        {3, 6},
        {5, 5},
        {6, 4},
        {3, 1},
        {0, 0},
        {1, 4},
    };
    Polygon* ppoly = new_polygon(vertices, 6);
    TriangleMesh* pscene = triangulate(ppoly);
    free_polygon(ppoly);
    return pscene;
}
