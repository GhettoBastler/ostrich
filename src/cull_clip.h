#ifndef CULL_CLIP_H
#define CULL_CLIP_H

#include "camera.h"
#include "primitives.h"

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

#endif
