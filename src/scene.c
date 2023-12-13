#include <stdlib.h>
#include <time.h>
#include "transforms.h"
#include "primitives.h"
#include "stl.h"

#define STL_FILE "input.stl"

TriangleMesh* tri_make_scene(){
    FILE* pfile = fopen(STL_FILE, "r");
    if (pfile == NULL){
        printf("No such file\n");
        exit(1);
    }
    TriangleMesh* pscene = stl_to_tri_mesh(pfile);
    fclose(pfile);
    return pscene;
}
