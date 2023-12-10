#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "camera.h"

Camera make_camera(float x, float y, float z, float rx, float ry, float rz, float focal_length){
    Camera cam;
    cam.translation.x = x;
    cam.translation.y = y;
    cam.translation.z = z;
    cam.rotation.x = rx;
    cam.rotation.y = ry;
    cam.rotation.z = rz;
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

Edge2D project_edge(Edge3D edge, float focal_length){
    Point2D a = project_point(edge.a, focal_length);
    Point2D b = project_point(edge.b, focal_length);
    Edge2D res = {a, b};
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
                                float r_x, float r_y, float r_z, 
                                float t_x, float t_y, float t_z){

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

void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, Camera* pcam){
    int n = 0;
    for (int i = 0; i < pmesh->size; i++){
        Point3D a = transform_point(pcam->transform_mat, pmesh->edges[i].a);
        Point3D b = transform_point(pcam->transform_mat, pmesh->edges[i].b);

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
        pbuffer->edges[n] = project_edge(new_edge, pcam->focal_length);
        n += 1;
    }
    pbuffer->size = n;
}
