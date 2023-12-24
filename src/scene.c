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

    float offset_x,
          offset_y = 0;

    for (int i = 0; i < 10; i++){
        offset_x = 0;
        for (int j = 0; j < 10; j++){
            pprism = extrude(ppoly, 100);
            for (int k = 0; k < pprism->size; k++){
                pprism->triangles[k].a.x += offset_x;
                pprism->triangles[k].a.y += offset_y;
                pprism->triangles[k].b.x += offset_x;
                pprism->triangles[k].b.y += offset_y;
                pprism->triangles[k].c.x += offset_x;
                pprism->triangles[k].c.y += offset_y;
            }
            pscene = merge_tri_meshes(pscene, pprism);
            offset_x += 100;
        }
        offset_y += 100;
    }
    free_polygon(ppoly);
    return pscene;
}
