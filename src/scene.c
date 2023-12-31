#include <stdlib.h>
#include <time.h>
#include "vect.h"
#include "transforms.h"
#include "primitives.h"
#include "stl.h"
#include "interpreter.h"

float get_random_float(float limit){
    return (float)rand()/(float)(RAND_MAX/(limit));
}

TriangleMesh* tri_make_scene(){
//     TriangleMesh* pscene = new_triangle_mesh();
//     srand(time(NULL));
// 
//     Point3D translation = {0, 0, 0};
//     Point3D rotation = {0, 0, 0};
//     TriangleMesh* pbox;
// 
//     for (int i = 0; i < 5; i++){
//         translation.x = 0;
//         for (int j = 0; j < 5; j++){
//             rotation.x = get_random_float(M_PI*2);
//             rotation.y = get_random_float(M_PI*2);
//             rotation.z = get_random_float(M_PI*2);
//             pbox = box(30, 20, 50);
//             rotate_mesh(pbox, rotation);
//             translate_mesh(pbox, translation);
//             pscene = merge_tri_meshes(pscene, pbox);
//             translation.x += 100;
//         }
//         translation.y += 100;
//     }
    TriangleMesh* pscene = parse_file();
    return pscene;
}
