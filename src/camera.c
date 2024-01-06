#define EPSILON 0.0005 // Arbitrary value to avoid lines intersecting with their own faces

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "camera.h"
#include "primitives.h"
#include "transforms.h"
#include "vect.h"
#include "utils.h"

int comp_tri_z(const void* ptri_a, const void* ptri_b);
void z_sort_triangles(TriangleMesh* pmesh);
bool facing_camera(Triangle tri);


Camera make_camera(float width, float height, float focal_length){
    Camera cam;
    cam.focal_length = focal_length;
    cam.width = width;
    cam.height = height;
    cam.orbit_radius = 0;

    for (int i = 0; i < 16; i++)
        cam.transform_mat[i] = 0;

    cam.transform_mat[0] = cam.transform_mat[5]
                         = cam.transform_mat[10]
                         = cam.transform_mat[15] = 1;

    return cam;
}

void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius){
    float new_mat[16],
          tmp_mat[16];
    if (orbit){
        Point3D z_translate = {0, 0, -orbit_radius};
        // Bringing the model to the camera
        calculate_translation_matrix(new_mat, z_translate);
        // Rotating it
        calculate_rotation_matrix(tmp_mat, rotation);
        multiply_matrix(new_mat, tmp_mat);
        // Putting it back
        calculate_translation_matrix(tmp_mat, pt_mul(-1.0, z_translate));
        multiply_matrix(new_mat, tmp_mat);
    } else {
        calculate_rotation_matrix(new_mat, rotation);
    }
    // Translating
    calculate_translation_matrix(tmp_mat, translation);
    multiply_matrix(new_mat, tmp_mat);
    multiply_matrix(mat, new_mat);
}

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

TriangleMesh* bface_cull(TriangleMesh* ptri){
    TriangleMesh* pres = new_triangle_mesh(0);
    Triangle curr_tri;

    for (int i = 0; i < ptri->size; i++){
        curr_tri = ptri->triangles[i];
        if (facing_camera(curr_tri)){
            pres = add_triangle(pres, curr_tri);
        }
    }
    return pres;
}

TriangleMesh* frustum_cull(TriangleMesh* ptri, Camera* pcam){
    TriangleMesh* pres = new_triangle_mesh(0);
    Triangle curr_tri;
    Point2D a_proj, b_proj, c_proj;

    for (int i = 0; i < ptri->size; i++){
        curr_tri = ptri->triangles[i];
        // Are all three vertices behind the focal plan ?
        if (curr_tri.a.z < pcam->focal_length &&
            curr_tri.b.z < pcam->focal_length &&
            curr_tri.c.z < pcam->focal_length)
            continue;

        // Projecting the vertices
        a_proj = project_point(curr_tri.a, pcam);
        b_proj = project_point(curr_tri.b, pcam);
        c_proj = project_point(curr_tri.c, pcam);

        // Are all three vertices left of the frustum ?
        if (a_proj.x < -pcam->width/2 &&
            b_proj.x < -pcam->width/2 &&
            c_proj.x < -pcam->width/2) 
            continue;
 
        // Are all three vertices right of the frustum ?
        if (a_proj.x > pcam->width/2 &&
            b_proj.x > pcam->width/2 &&
            c_proj.x > pcam->width/2) 
            continue;
 
        // Are all three vertices above the frustum ?
        if (a_proj.y < -pcam->width/2 &&
            b_proj.y < -pcam->width/2 &&
            c_proj.y < -pcam->width/2) 
            continue;

        // Are all three vertices above the frustum ?
        if (a_proj.y > pcam->width/2 &&
            b_proj.y > pcam->width/2 &&
            c_proj.y > pcam->width/2) 
            continue;

        // The triangle is inside the frustum, add it
        pres = add_triangle(pres, curr_tri);
    }
    return pres;
}

void z_sort_triangles(TriangleMesh* pmesh){
    qsort(pmesh->triangles, pmesh->size, sizeof(Triangle), comp_tri_z);
}
