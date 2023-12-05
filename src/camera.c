#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "camera.h"

// Functions
// Point transformation
Point3D rotate_point(Point3D point, Camera* pcam){
    Point3D res;

    res.x = pcam->rot_mat[0] * point.x + pcam->rot_mat[1] * point.y + pcam->rot_mat[2] * point.z;
    res.y = pcam->rot_mat[3] * point.x + pcam->rot_mat[4] * point.y + pcam->rot_mat[5] * point.z;
    res.z = pcam->rot_mat[6] * point.x + pcam->rot_mat[7] * point.y + pcam->rot_mat[8] * point.z;

    return res;
}

Point3D translate_point(Point3D point, Point3D vector){
    Point3D res;

    res.x = point.x + vector.x;
    res.y = point.y + vector.y;
    res.z = point.z + vector.z;

    return res;
}

// 2D projection
Point2D project_point(Point3D point, float dist, float focal_length){
    float x = (point.x * (focal_length / (point.z))) + (WIDTH / 2);
    float y = (point.y * (focal_length / (point.z))) + (HEIGHT / 2);
    Point2D res = {x, y};
    return res;
}

Edge2D project_edge(Edge3D edge, float dist, float focal_length){
    Point2D a = project_point(edge.a, dist, focal_length);
    Point2D b = project_point(edge.b, dist, focal_length);
    Edge2D res = {a, b};
    return res;
}

void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, Camera* pcam){
    int n = 0;
    for (int i = 0; i < pmesh->size; i++){
        Point3D a_trans, b_trans;
        Point3D a = rotate_point(pmesh->edges[i].a, pcam);
        a = translate_point(a, pcam->translation);
        Point3D b = rotate_point(pmesh->edges[i].b, pcam);
        b = translate_point(b, pcam->translation);

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
        pbuffer->edges[n] = project_edge(new_edge, pcam->translation.z, pcam->focal_length);
        n += 1;
    }
    pbuffer->size = n;
}

// Interface
void update_rotation_matrix(Camera* pcam){
    Point3D r = pcam->rotation;
    pcam->rot_mat[0] = cosf(r.x) * cosf(r.y);
    pcam->rot_mat[1] = cosf(r.x) * sinf(r.y) * sinf(r.z) - sinf(r.x) * cosf(r.z);
    pcam->rot_mat[2] = cosf(r.x) * sinf(r.y) * cosf(r.z) + sinf(r.x) * sinf(r.z);
    pcam->rot_mat[3] = sinf(r.x) * cosf(r.y);
    pcam->rot_mat[4] = sinf(r.x) * sinf(r.y) * sinf(r.z) + cosf(r.x) * cosf(r.z);
    pcam->rot_mat[5] = sinf(r.x) * sinf(r.y) * cosf(r.z) - cosf(r.x) * sinf(r.z);
    pcam->rot_mat[6] = -sinf(r.y);
    pcam->rot_mat[7] = cosf(r.y) * sinf(r.z);
    pcam->rot_mat[8] = cosf(r.y) * cosf(r.z);
}
