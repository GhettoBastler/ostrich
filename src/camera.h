#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include "primitives.h"

#define WIDTH 720
#define HEIGHT 480
#define SCALE 80

typedef struct {
    float width, height;
    float focal_length;
    float transform_mat[16];
    float orbit_radius;
} Camera;

void z_sort_triangles(TriangleMesh* pmesh);
TriangleMesh* bface_cull(TriangleMesh* ptri);
TriangleMesh* frustum_cull(TriangleMesh* ptri, Camera* pcam);
Camera make_camera(float width, float height, float focal_length);
void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius);

#endif
