#include <stdlib.h>
#include <time.h>
#include "vect.h"
#include "transforms.h"
#include "primitives.h"
#include "stl.h"

float get_random_float(float limit){
    return (float)rand()/(float)(RAND_MAX/(limit));
}

TriangleMesh* tri_make_scene(){
    srand(time(NULL));
    Polygon* ppoly;
    TriangleMesh* pprism;
    TriangleMesh* pscene = new_triangle_mesh();

    ppoly = new_regular_polygon(50, 5);
    Point3D translation = {0, 0, 0};
    Point3D rotation = {0, 0, 0};

    for (int i = 0; i < 5; i++){
        translation.x = 0;
        for (int j = 0; j < 5; j++){
            translation.z = get_random_float(500);
            rotation.x = get_random_float(M_PI*2);
            rotation.y = get_random_float(M_PI*2);
            rotation.z = get_random_float(M_PI*2);
            pprism = extrude(ppoly, get_random_float(200) + 50);
            rotate_mesh(pprism, rotation);
            translate_mesh(pprism, translation);
            pscene = merge_tri_meshes(pscene, pprism);
            translation.x += 300;
        }
        translation.y += 300;
    }
    free_polygon(ppoly);
    return pscene;
}
