#include <stdlib.h>
#include <time.h>
#include "vect.h"
#include "transforms.h"
#include "primitives.h"
#include "stl.h"

TriangleMesh* tri_make_scene(){
    Polygon* ppoly;
    TriangleMesh* pprism;
    TriangleMesh* pscene = new_triangle_mesh();

    ppoly = new_regular_polygon(50, 5);
    Point3D translation = {0, 0, 0};

    for (int i = 0; i < 5; i++){
        translation.x = 0;
        for (int j = 0; j < 5; j++){
            pprism = extrude(ppoly, 100);
            translate_mesh(pprism, translation);
            pscene = merge_tri_meshes(pscene, pprism);
            translation.x += 100;
        }
        translation.y += 100;
    }
    free_polygon(ppoly);
    return pscene;
}
