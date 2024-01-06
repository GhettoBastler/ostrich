#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include "primitives.h"

#define LINE_COLOR_1 0xFFF4115D
#define LINE_COLOR_2 0xFF000000
#define BG_COLOR 0xFFFFFFFF
#define WIDTH 720
#define HEIGHT 480
#define SCALE 80

typedef struct {
    float width, height;
    float focal_length;
    float transform_mat[16];
    float orbit_radius;
} Camera;

typedef struct {
    Point3D min, max;
} BoundingBox;

void z_sort_triangles(TriangleMesh* pmesh);
void clip_frustum(Edge3D* edge, Camera* pcam);
TriangleMesh* bface_cull(TriangleMesh* ptri);
TriangleMesh* frustum_cull(TriangleMesh* ptri, Camera* pcam);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* pmesh, int start_idx);
BoundingBox bbox_from_triangle(Triangle triangle);
BoundingBox bbox_from_edge(Edge3D edge);
float obj_ratio_from_screen_ratio(Edge3D edge3D, Edge2D edge2D, float focal_length,
                                  float ratio, bool reverse);

void draw_line(uint32_t* ppixels, ProjectedEdge edge, TriangleMesh* pmesh,
               bool draw_hidden, Camera* pcam);
Point2D project_point(Point3D point, Camera* pcam);
Camera make_camera(float width, float height, float focal_length);
void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius);
TriangleMesh* project_tri_mesh(ProjectedMesh* pbuffer, TriangleMesh* pmesh, Camera* pcam, bool do_cull);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* pmesh, int start_idx);

#endif
