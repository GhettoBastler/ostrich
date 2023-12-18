#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cull_clip.h"
#include "camera.h"
#include "vect.h"
#include "transforms.h"

bool facing_camera(Triangle tri){
    Point3D vect_1 = pt_diff(tri.b, tri.a),
            vect_2 = pt_diff(tri.a, tri.c);
    Point3D normal = cross_product(vect_1, vect_2);
    Point3D center = pt_mul((float)1/3, pt_add(pt_add(tri.a, tri.b), tri.c));
    return (dot_product(center, normal) >= 0);
}



int comp_tri_z(const void* ptri_a, const void* ptri_b){
    Point3D tri_a_a = ((Triangle*) ptri_a)->a,
            tri_a_b = ((Triangle*) ptri_a)->b,
            tri_a_c = ((Triangle*) ptri_a)->c,
            tri_b_a = ((Triangle*) ptri_b)->a,
            tri_b_b = ((Triangle*) ptri_b)->b,
            tri_b_c = ((Triangle*) ptri_b)->c;

    float min_z_a = fminf(fminf(tri_a_a.z, tri_a_b.z), tri_a_c.z),
          min_z_b = fminf(fminf(tri_b_a.z, tri_b_b.z), tri_b_c.z);

    if (min_z_a < min_z_b)
        return -1;
    else if (min_z_a == min_z_b)
        return 0;
    else
        return 1;
}

TriangleMesh* bface_cull(float* matrix, TriangleMesh* ptri){
    TriangleMesh* pres = (TriangleMesh*) malloc(sizeof(TriangleMesh));
    pres->size = 0;
    Point3D trans_a, trans_b, trans_c;
    Triangle trans_tri;
    Edge3D ab, bc, ca;

    for (int i = 0; i < ptri->size; i++){
        trans_a = transform_point(matrix, ptri->triangles[i].a);
        trans_b = transform_point(matrix, ptri->triangles[i].b);
        trans_c = transform_point(matrix, ptri->triangles[i].c);

        trans_tri.a = trans_a;
        trans_tri.b = trans_b;
        trans_tri.c = trans_c;
        memcpy(trans_tri.visible, ptri->triangles[i].visible, 3);

        if (facing_camera(trans_tri)){
            pres = add_triangle(pres, trans_tri);
        }
    }

    // Z-sort triangles
    qsort(pres->triangles, pres->size, sizeof(Triangle), comp_tri_z);
    return pres;
}
