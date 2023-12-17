#ifndef CAMERA_H
#define CAMERA_H

#include "primitives.h"

#define WIDTH 800
#define HEIGHT 600

typedef struct {
//    Point3D translation, rotation;
    float focal_length;
    float transform_mat[16];
} Camera;

Camera make_camera(float x, float y, float z, float rx, float ry, float rz, float focal_length);
void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius);
TriangleMesh* project_tri_mesh(ProjectedMesh* pbuffer, TriangleMesh* pmesh, Camera* pcam);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* pmesh, int start_idx);

#endif
