#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "camera.h"
#include "cull_clip.h"
#include "transforms.h"
#include "vect.h"

Camera make_camera(float width, float height, float focal_length){
    Camera cam;
    cam.focal_length = focal_length;
    cam.width = width;
    cam.height = height;

    for (int i = 0; i < 16; i++)
        cam.transform_mat[i] = 0;

    cam.transform_mat[0] = cam.transform_mat[5]
                         = cam.transform_mat[10]
                         = cam.transform_mat[15] = 1;

    return cam;
}

// 2D projection
Point2D project_point(Point3D point, Camera* pcam){
    float x = point.x * (pcam->focal_length / (point.z));
    float y = point.y * (pcam->focal_length / (point.z));
    Point2D res = {x, y};
    return res;
}

ProjectedEdge project_edge(Edge3D edge, Camera* pcam){
    ProjectedEdge res;
    Point2D a = project_point(edge.a, pcam);
    Point2D b = project_point(edge.b, pcam);
    res.edge3D = edge;
    res.edge2D.a = a;
    res.edge2D.b = b;

    return res;
}

// Homogeneous coordinates transforms
Point3D transform_point(float* matrix, Point3D point){
    Point3D res;
    res.x = point.x * matrix[0] + point.y * matrix[1] + point.z * matrix[2] + matrix[3];
    res.y = point.x * matrix[4] + point.y * matrix[5] + point.z * matrix[6] + matrix[7];
    res.z = point.x * matrix[8] + point.y * matrix[9] + point.z * matrix[10] + matrix[11];
    return res;
}

void calculate_rotation_matrix(float* matrix, Point3D rotation){
    float r_x = rotation.x,
          r_y = rotation.y,
          r_z = rotation.z;

    matrix[0] = cosf(r_y) * cosf(r_z);
    matrix[1] = sinf(r_x) * sinf(r_y) * cosf(r_z) - cosf(r_x) * sinf(r_z);
    matrix[2] = cosf(r_x) * sinf(r_y) * cosf(r_z) + sinf(r_x) * sinf(r_z);
    matrix[4] = cosf(r_y) * sinf(r_z);
    matrix[5] = sinf(r_x) * sinf(r_y) * sinf(r_z) + cosf(r_x) * cosf(r_z);
    matrix[6] = cosf(r_x) * sinf(r_y) * sinf(r_z) - sinf(r_x) * cosf(r_z);
    matrix[8] = -sinf(r_y);
    matrix[9] = sinf(r_x) * cosf(r_y);
    matrix[10] = cosf(r_x) * cosf(r_y);

    matrix[3] = matrix[7]
              = matrix[11]
              = matrix[12]
              = matrix[13]
              = matrix[14] = 0;
    matrix[15] = 1;
}

void calculate_translation_matrix(float* matrix, Point3D translation){
    matrix[3] = translation.x;
    matrix[7] = translation.y;
    matrix[11] = translation.z;

    matrix[1] = matrix[2]
              = matrix[4]
              = matrix[6]
              = matrix[8]
              = matrix[9]
              = matrix[12]
              = matrix[13]
              = matrix[14] = 0;

    matrix[0] = matrix[5]
              = matrix[10]
              = matrix[15] = 1;
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

TriangleMesh* project_tri_mesh(ProjectedMesh* pbuffer, TriangleMesh* ptri_mesh, Camera* pcam){
    TriangleMesh* pculled_tri = bface_cull(pcam->transform_mat, ptri_mesh);
    Edge3D edges[3];
    ProjectedEdge curr_proj_edge;

    int n = 0;
    for (int i = 0; i < pculled_tri->size; i++){
        // Convert each triangle into three edges
        // AB
        edges[0].a = pculled_tri->triangles[i].a;
        edges[0].b = pculled_tri->triangles[i].b;
        // BC
        edges[1].a = pculled_tri->triangles[i].b;
        edges[1].b = pculled_tri->triangles[i].c;
        // CA
        edges[2].a = pculled_tri->triangles[i].c;
        edges[2].b = pculled_tri->triangles[i].a;

        for (int j = 0; j < 3; j++){
            // Add only the visible edges
            if (pculled_tri->triangles[i].visible[j]) {
                // Clip the line if it goes outside the frustum
                clip_frustum(&edges[j], pcam);
                // If there is nothing left
                if (pt_is_null(edges[j].a) && pt_is_null(edges[j].b))
                    continue;
                // Project the edge
                curr_proj_edge = project_edge(edges[j], pcam);
                // Add it to the mesh
                pbuffer->edges[n] = curr_proj_edge;
                n += 1;
            }
        }
    }
    pbuffer->size = n;
    return pculled_tri;
}

Triangle transform_triangle(float* matrix, Triangle tri){
    Point3D trans_a, trans_b, trans_c;
    Triangle trans_tri;

    trans_a = transform_point(matrix, tri.a);
    trans_b = transform_point(matrix, tri.b);
    trans_c = transform_point(matrix, tri.c);

    trans_tri.a = trans_a;
    trans_tri.b = trans_b;
    trans_tri.c = trans_c;
    memcpy(trans_tri.visible, trans_tri.visible, 3);

    return trans_tri;
}
