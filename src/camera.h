#ifndef CAMERA_H
#define CAMERA_H

#include "primitives.h"

#define WIDTH 800
#define HEIGHT 600

typedef struct {
    Point3D translation, rotation;
    float focal_length;
    float rot_mat[9];
} Camera;

Point3D rotate_point(Point3D point, Camera* pcam);
Point3D translate_point(Point3D point, Point3D vector);
Point2D project_point(Point3D point, float dist, float focal_length);
Edge2D project_edge(Edge3D edge, float dist, float focal_length);
void project_mesh(Mesh2D* pbuffer, Mesh3D* pmesh, Camera* pcam);
void update_rotation_matrix(Camera* pcam);

#endif
