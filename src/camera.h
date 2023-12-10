#ifndef CAMERA_H
#define CAMERA_H

#include "primitives.h"

#define WIDTH 800
#define HEIGHT 600

typedef struct {
    Point3D translation, rotation;
    float focal_length;
    float transform_mat[16];
} Camera;

Camera make_camera(float x, float y, float z, float rx, float ry, float rz, float focal_length);
void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, Camera* pcam);
void calculate_transform_matrix(float* matrix,
                                float r_x, float r_y, float r_z,
                                float t_x, float t_y, float t_z);
void multiply_matrix(float* matA, float* matB);

#endif
