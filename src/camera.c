#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "camera.h"
#include "transforms.h"
#include "vect.h"

Camera make_camera(float x, float y, float z, float rx, float ry, float rz, float focal_length){
    Camera cam;
    cam.focal_length = focal_length;

    for (int i = 0; i < 16; i++)
        cam.transform_mat[i] = 0;
    cam.transform_mat[0] = cam.transform_mat[5]
                         = cam.transform_mat[10]
                         = cam.transform_mat[15] = 1;

    return cam;
}

// 2D projection
Point2D project_point(Point3D point, float focal_length){
    float x = (point.x * (focal_length / (point.z))) + (WIDTH / 2);
    float y = (point.y * (focal_length / (point.z))) + (HEIGHT / 2);
    Point2D res = {x, y};
    return res;
}

ProjectedEdge project_edge(Edge3D edge, float focal_length){
    ProjectedEdge res;
    Point2D a = project_point(edge.a, focal_length);
    Point2D b = project_point(edge.b, focal_length);
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

void calculate_transform_matrix(float* matrix,
                                Point3D rotation,
                                Point3D translation){

    float r_x = rotation.x,
          r_y = rotation.y,
          r_z = rotation.z,
          t_x = translation.x,
          t_y = translation.y,
          t_z = translation.z;

    float r0, r1, r2, r3, r4, r5, r6, r7, r8;

    r0 = cosf(r_y) * cosf(r_z);
    r1 = sinf(r_x) * sinf(r_y) * cosf(r_z) - cosf(r_x) * sinf(r_z);
    r2 = cosf(r_x) * sinf(r_y) * cosf(r_z) + sinf(r_x) * sinf(r_z);
    r3 = cosf(r_y) * sinf(r_z);
    r4 = sinf(r_x) * sinf(r_y) * sinf(r_z) + cosf(r_x) * cosf(r_z);
    r5 = cosf(r_x) * sinf(r_y) * sinf(r_z) - sinf(r_x) * cosf(r_z);
    r6 = -sinf(r_y);
    r7 = sinf(r_x) * cosf(r_y);
    r8 = cosf(r_x) * cosf(r_y);

    matrix[0] = r0;
    matrix[1] = r1;
    matrix[2] = r2;
    matrix[4] = r3;
    matrix[5] = r4;
    matrix[6] = r5;
    matrix[8] = r6;
    matrix[9] = r7;
    matrix[10] = r8;

    matrix[3] = r0 * t_x + r1 * t_y + r2 * t_z;
    matrix[7] = r3 * t_x + r4 * t_y + r5 * t_z;
    matrix[11] = r6 * t_x + r7 * t_y + r8 * t_z;

    matrix[12] = matrix[13] = matrix[14] = 0;
    matrix[15] = 1;
}

void multiply_matrix(float* matB, float* matA){
    float res[16];
    res[0] = matA[0] * matB[0] + matA[1] * matB[4] + matA[2] * matB[8] + matA[3] * matB[12];
    res[1] = matA[0] * matB[1] + matA[1] * matB[5] + matA[2] * matB[9] + matA[3] * matB[13];
    res[2] = matA[0] * matB[2] + matA[1] * matB[6] + matA[2] * matB[10] + matA[3] * matB[14];
    res[3] = matA[0] * matB[3] + matA[1] * matB[7] + matA[2] * matB[11] + matA[3] * matB[15];
    res[4] = matA[4] * matB[0] + matA[5] * matB[4] + matA[6] * matB[8] + matA[7] * matB[12];
    res[5] = matA[4] * matB[1] + matA[5] * matB[5] + matA[6] * matB[9] + matA[7] * matB[13];
    res[6] = matA[4] * matB[2] + matA[5] * matB[6] + matA[6] * matB[10] + matA[7] * matB[14];
    res[7] = matA[4] * matB[3] + matA[5] * matB[7] + matA[6] * matB[11] + matA[7] * matB[15];
    res[8] = matA[8] * matB[0] + matA[9] * matB[4] + matA[10] * matB[8] + matA[11] * matB[12];
    res[9] = matA[8] * matB[1] + matA[9] * matB[5] + matA[10] * matB[9] + matA[11] * matB[13];
    res[10] = matA[8] * matB[2] + matA[9] * matB[6] + matA[10] * matB[10] + matA[11] * matB[14];
    res[11] = matA[8] * matB[3] + matA[9] * matB[7] + matA[10] * matB[11] + matA[11] * matB[15];
    res[12] = matA[12] * matB[0] + matA[13] * matB[4] + matA[14] * matB[8] + matA[15] * matB[12];
    res[13] = matA[12] * matB[1] + matA[13] * matB[5] + matA[14] * matB[9] + matA[15] * matB[13];
    res[14] = matA[12] * matB[2] + matA[13] * matB[6] + matA[14] * matB[10] + matA[15] * matB[14];
    res[15] = matA[12] * matB[3] + matA[13] * matB[7] + matA[14] * matB[11] + matA[15] * matB[15];

    memcpy(matB, res, sizeof(float) * 16);
}

void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius){
    float new_mat[16];
    if (orbit){
        float tmp_mat[16];
        Point3D nul_pt = {0, 0, 0},
                z_translate = {0, 0, -orbit_radius};

        // Bringing the model to the camera
        calculate_transform_matrix(new_mat,
            nul_pt,
            z_translate);
        // Rotating it
        calculate_transform_matrix(tmp_mat,
            rotation,
            nul_pt);
        multiply_matrix(new_mat, tmp_mat);
        // Putting it back
        calculate_transform_matrix(tmp_mat,
            nul_pt,
            pt_mul(-1.0, z_translate));
        multiply_matrix(new_mat, tmp_mat);
        // Translating
        calculate_transform_matrix(tmp_mat,
            nul_pt,
            translation);
        multiply_matrix(new_mat, tmp_mat);
    } else {
        calculate_transform_matrix(new_mat,
               rotation,
               translation);
    }
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
                // Clip edges that go behind the camera
                Point3D a = edges[j].a;
                Point3D b = edges[j].b;

                // Check visibility
                bool a_hidden, b_hidden;
                a_hidden = a.z <= 0;
                b_hidden = b.z <= 0;

                if (a_hidden && b_hidden){
                    // Both points are hidden
                    // Skip this edge
                    continue;
                } else if (a_hidden){
                    // A is hidden, B is visible
                    a.x = (a.z / (b.z - a.z)) * (b.x - a.x) - a.x;
                    a.y = (a.z / (b.z - a.z)) * (b.y - a.y) - a.y;
                    a.z = -1;
                } else if (b_hidden){
                    // B is hidden, A is visible
                    b.x = (b.z / (a.z - b.z)) * (a.x - b.x) - b.x;
                    b.y = (b.z / (a.z - b.z)) * (a.y - b.y) - b.y;
                    b.z = -1;
                } // If both are visible, we do nothing

                Edge3D new_edge = {a, b};
                curr_proj_edge = project_edge(new_edge, pcam->focal_length);

                // Clip edges that go beyond the screen limit, both in screen and object space

                Point2D a2 = curr_proj_edge.edge2D.a;
                Point2D b2 = curr_proj_edge.edge2D.b;

                float dx3 = b.x - a.x;
                float dy3 = b.y - a.y;
                float dz3 = b.z - a.z;

                float dx = b2.x - a2.x;
                float dy = b2.y - a2.y;

                float ratio;

                if (dx != 0){
                    if (a2.x < 0){
                        ratio = -(a2.x / dx);
                        a2.y = -(a2.x / dx) * dy + a2.y;
                        a2.x = 0;

                        a.x = ratio * dx3 + a.x;
                        a.y = ratio * dy3 + a.y;
                        a.z = ratio * dz3 + a.z;
                    } else if (a2.x > WIDTH){
                        ratio = -(a2.x - WIDTH)/ dx;
                        a2.y = -((a2.x - WIDTH) / dx) * dy + a2.y;
                        a2.x = WIDTH;

                        a.x = ratio * dx3 + a.x;
                        a.y = ratio * dy3 + a.y;
                        a.z = ratio * dz3 + a.z;
                    }

                    if (b2.x < 0){
                        ratio = -b2.x / dx;
                        b2.y = -(b2.x / dx) * dy + b2.y;
                        b2.x = 0;

                        b.x = ratio * dx3 + b.x;
                        b.y = ratio * dy3 + b.y;
                        b.z = ratio * dz3 + b.z;
                    } else if (b2.x > WIDTH){
                        ratio = -(b2.x - WIDTH)/ dx;
                        b2.y = -((b2.x - WIDTH) / dx) * dy + b2.y;
                        b2.x = WIDTH;

                        b.x = ratio * dx3 + b.x;
                        b.y = ratio * dy3 + b.y;
                        b.z = ratio * dz3 + b.z;
                    }
                }
                
                if (dy != 0){
                    if (a2.y < 0){
                        ratio = -(a2.y / dy);
                        a2.x = -(a2.y / dy) * dx + a2.x;
                        a2.y = 0;

                        a.x = ratio * dx3 + a.x;
                        a.y = ratio * dy3 + a.y;
                        a.z = ratio * dz3 + a.z;
                    } else if (a2.y > HEIGHT){
                        ratio = -(a2.y - HEIGHT) / dy;
                        a2.x = -((a2.y - HEIGHT) / dy) * dx + a2.x;
                        a2.y = HEIGHT;

                        a.x = ratio * dx3 + a.x;
                        a.y = ratio * dy3 + a.y;
                        a.z = ratio * dz3 + a.z;
                    }

                    if (b2.y < 0){
                        ratio = -b2.y/ dy;
                        b2.x = -(b2.y / dy) * dx + b2.x;
                        b2.y = 0;

                        b.x = ratio * dx3 + b.x;
                        b.y = ratio * dy3 + b.y;
                        b.z = ratio * dz3 + b.z;
                    } else if (b2.y > HEIGHT){
                        ratio = -(b2.y - HEIGHT) / dy;
                        b2.x = -((b2.y - HEIGHT) / dy) * dx + b2.x;
                        b2.y = HEIGHT;

                        b.x = ratio * dx3 + b.x;
                        b.y = ratio * dy3 + b.y;
                        b.z = ratio * dz3 + b.z;
                    }
                }

                // Updating the clipped coordinates
                curr_proj_edge.edge3D.a = a;
                curr_proj_edge.edge3D.b = b;
                curr_proj_edge.edge2D.a = a2;
                curr_proj_edge.edge2D.b = b2;
                pbuffer->edges[n] = curr_proj_edge;
                n += 1;
            }
        }
    }
    pbuffer->size = n;
    return pculled_tri;
}

bool ray_tri_intersect(Point3D* inter, Point3D point, Triangle tri){
    Point3D ab = pt_diff(tri.b, tri.a),
            bc = pt_diff(tri.c, tri.b),
            ca = pt_diff(tri.a, tri.c),
            normal = cross_product(ab, bc);

    float q = dot_product(normal, tri.a) / dot_product(normal, point);

    *inter = pt_mul(q, point);

    return (cross_product(pt_diff(*inter, tri.a), ab).z > 0 &&
            cross_product(pt_diff(*inter, tri.b), bc).z > 0 &&
            cross_product(pt_diff(*inter, tri.c), ca).z > 0);
}

bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* ptri_mesh, int start_idx){
    Point3D pt_obj = pt_add(pt_mul(ratio, edge.b),
                            pt_mul((1-ratio), edge.a));

    Triangle curr_tri;
    Point3D intersect;
    Point3D bbox_min, bbox_max;

    for (int i = start_idx; i<ptri_mesh->size; i++){
        curr_tri = ptri_mesh->triangles[i];
        bbox_min = pt_min(pt_min(curr_tri.a, curr_tri.b), curr_tri.c);
        bbox_max = pt_max(pt_max(curr_tri.a, curr_tri.b), curr_tri.c);

        // If point is totally in front of curr_tri, it doesn't intersect it
        if (pt_obj.z < bbox_min.z)
            return true;
        
        // If point is projected outside of the triangle bounding box, it doesn't hide it
        if (
           (
            (pt_obj.x * bbox_max.z / pt_obj.z < bbox_min.x) &&
            (pt_obj.x * bbox_min.z / pt_obj.z < bbox_min.x)
           ) || (
            (pt_obj.x * bbox_max.z / pt_obj.z > bbox_max.x) &&
            (pt_obj.x * bbox_min.z / pt_obj.z > bbox_max.x)
           ) || (
            (pt_obj.y * bbox_max.z / pt_obj.z < bbox_min.y) &&
            (pt_obj.y * bbox_min.z / pt_obj.z < bbox_min.y)
           ) || (
            (pt_obj.y * bbox_max.z / pt_obj.z > bbox_max.y) &&
            (pt_obj.y * bbox_min.z / pt_obj.z > bbox_max.y)
           )
        )
            continue;

        if (ray_tri_intersect(&intersect, pt_obj, curr_tri)){
            if (intersect.z + 0.0001 < pt_obj.z){ // Adding an arbitrary value to make sure that edges don't intersect their own faces
                return false;
            }
        }
    }
    return true;
}
