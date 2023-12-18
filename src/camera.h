#ifndef CAMERA_H
#define CAMERA_H

#include "primitives.h"

#define WIDTH 800
#define HEIGHT 600
#define SCALE 40

typedef struct {
    float width, height;
    float focal_length;
    float transform_mat[16];
} Camera;

Camera make_camera(float width, float height, float focal_length);
void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius);
TriangleMesh* project_tri_mesh(ProjectedMesh* pbuffer, TriangleMesh* pmesh, Camera* pcam);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* pmesh, int start_idx);

#endif
