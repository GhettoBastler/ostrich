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

//Edge2D project_edge(Edge3D edge, float focal_length){
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

void update_transform_matrix(float* mat, Point3D rotation, Point3D translation){
    float new_mat[16];
    calculate_transform_matrix(new_mat,
           rotation,
           translation);
    multiply_matrix(mat, new_mat);
}

void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, Camera* pcam){
    int n = 0;
    ProjectedEdge curr_proj_edge;

    for (int i = 0; i < pmesh->size; i++){
        Point3D a = transform_point(pcam->transform_mat, pmesh->edges[i].a);
        Point3D b = transform_point(pcam->transform_mat, pmesh->edges[i].b);
        // Add camera offset
        a.x += WIDTH / 2;
        a.y += HEIGHT / 2;
        b.x += WIDTH / 2;
        b.y += HEIGHT / 2;

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
        // pbuffer->edges[n] = project_edge(new_edge, pcam->focal_length);
        curr_proj_edge = project_edge(new_edge, pcam->focal_length);
        curr_proj_edge.edge3D = new_edge;
        pbuffer->edges[n] = curr_proj_edge.edge2D;
        n += 1;
    }
    pbuffer->size = n;
}

// Point3D cross_product(Point3D a, Point3D b){
//     float res_x = a.y * b.z - a.z * b.y;
//    float res_y = a.z * b.x - a.x * b.z;
//    float res_z = a.x * b.y - a.y * b.x;
//    Point3D res = {res_x, res_y, res_z};
//    return res;
//}

bool facing_camera(Triangle tri){
    Point3D vect_1, vect_2;
    vect_1.x = tri.b.x - tri.a.x;
    vect_1.y = tri.b.y - tri.a.y;
    vect_1.z = tri.b.z - tri.a.z;
    vect_2.x = tri.c.x - tri.b.x;
    vect_2.y = tri.c.y - tri.b.y;
    vect_2.z = tri.c.z - tri.b.z;
    Point3D normal = cross_product(vect_1, vect_2);
    Point3D center = {(tri.a.x + tri.b.x + tri.c.x / 3),
                      (tri.a.y + tri.b.y + tri.c.y / 3),
                      (tri.a.z + tri.b.z + tri.c.z / 3)};

    float dot_product = center.x * normal.x + center.y * normal.y + center.z * normal.z;
    return (dot_product >= 0);
}

//Mesh3D* bface_cull(float* matrix, TriangleMesh* ptri){
TriangleMesh* bface_cull(float* matrix, TriangleMesh* ptri){
    //Mesh3D* pres = (Mesh3D*) malloc(sizeof(Mesh3D));
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

        if (facing_camera(trans_tri)){
            //ab.a = trans_a;
            //ab.b = trans_b;
            //bc.a = trans_b;
            //bc.b = trans_c;
            //ca.a = trans_c;
            //ca.b = trans_a;

            //if (ptri->triangles[i].visible[0])
            //    pres = add_edge(pres, ab);
            //if (ptri->triangles[i].visible[1])
            //    pres = add_edge(pres, bc);
            //if (ptri->triangles[i].visible[2])
            //    pres = add_edge(pres, ca);
            pres = add_triangle(pres, trans_tri);
        }
    }
    return pres;
}

void project_tri_mesh(Mesh2D* pbuffer, TriangleMesh* ptri_mesh, Camera* pcam){
    //Mesh3D* pculled = bface_cull(pcam->transform_mat, ptri_mesh);
    TriangleMesh* pculled_tri = bface_cull(pcam->transform_mat, ptri_mesh);
    Mesh3D* pculled = (Mesh3D*) malloc(sizeof(Mesh3D));
    pculled->size = 0;
    Edge3D ab, bc, ca;
    for (int j = 0; j < pculled_tri->size; j++){
        // Convert each triangle into three edges
        ab.a = pculled_tri->triangles[j].a;
        ab.b = pculled_tri->triangles[j].b;
        bc.a = pculled_tri->triangles[j].b;
        bc.b = pculled_tri->triangles[j].c;
        ca.a = pculled_tri->triangles[j].c;
        ca.b = pculled_tri->triangles[j].a;

        if (pculled_tri->triangles[j].visible[0])
            pculled = add_edge(pculled, ab);
        if (pculled_tri->triangles[j].visible[1])
            pculled = add_edge(pculled, bc);
        if (pculled_tri->triangles[j].visible[2])
            pculled = add_edge(pculled, ca);
    }
    int n = 0;
    ProjectedEdge curr_proj_edge;
    for (int i = 0; i < pculled->size; i++){
        Point3D a = pculled->edges[i].a;
        Point3D b = pculled->edges[i].b;

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
        //pbuffer->edges[n] = project_edge(new_edge, pcam->focal_length);
        curr_proj_edge = project_edge(new_edge, pcam->focal_length);
        curr_proj_edge.edge3D = new_edge;
        pbuffer->edges[n] = curr_proj_edge.edge2D;
        n += 1;
    }
    pbuffer->size = n;
    free(pculled_tri);
    free(pculled);
}

bool ray_tri_intersect(Point3D* inter, Point3D point, Triangle tri){
    Point3D ab = pt_diff(tri.b, tri.a),
            bc = pt_diff(tri.c, tri.b),
            ca = pt_diff(tri.a, tri.c),
            normal = cross_product(ab, bc);

    float q = dot_product(normal, tri.a) / dot_product(normal, point);

    *inter = pt_mul(q, point);

    return (dot_product(normal, cross_product(pt_diff(*inter, tri.a), ab)) >= 0 &&
            dot_product(normal, cross_product(pt_diff(*inter, tri.b), bc)) >= 0 &&
            dot_product(normal, cross_product(pt_diff(*inter, tri.c), ca)));
}

bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* ptri_mesh){
    Point3D pt_obj = pt_add(pt_mul(ratio, edge.b),
                            pt_mul((1-ratio), edge.a));

    Triangle curr_tri;
    Point3D intersect;
    for (int i = 0; i<ptri_mesh->size; i++){
        curr_tri = ptri_mesh->triangles[i];
        if (ray_tri_intersect(&intersect, pt_obj, curr_tri))
            if (intersect.z < pt_obj.z)
                return false;
    }
    return true;
}
