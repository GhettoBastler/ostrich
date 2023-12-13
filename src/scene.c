#include <stdlib.h>
#include <time.h>
#include "transforms.h"
#include "primitives.h"
#include "stl.h"

TriangleMesh* tri_make_scene(){
    //int offset_x = 0, offset_z = 0;
    //TriangleMesh* pres = tri_cube(10);
    //TriangleMesh* curr_cube;
    //for (int i = 0; i < 3; i++){
    //    offset_x = 0;
    //    for (int j = 0; j < 3; j++){
    //        curr_cube = tri_cube(10);
    //        for (int k = 0; k < curr_cube->size; k++){
    //            curr_cube->triangles[k].a.x += offset_x;
    //            curr_cube->triangles[k].b.x += offset_x;
    //            curr_cube->triangles[k].c.x += offset_x;

    //            curr_cube->triangles[k].a.z += offset_z;
    //            curr_cube->triangles[k].b.z += offset_z;
    //            curr_cube->triangles[k].c.z += offset_z;
    //        }
    //        pres = merge_tri_meshes(pres, curr_cube);
    //        offset_x += 20;
    //    }
    //    offset_z += 20;
    //}
    //return pres;

    FILE* pfile = fopen("input.stl", "r");
    if (pfile == NULL){
        printf("No such file\n");
        exit(1);
    }
    TriangleMesh* pscene = stl_to_tri_mesh(pfile);
    fclose(pfile);
    return pscene;
}
