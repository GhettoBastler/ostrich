#ifndef CULL_CLIP_H
#define CULL_CLIP_H

#include "camera.h"
#include "primitives.h"

bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* ptri_mesh, int start_idx);
void clip_frustum(Edge3D* edge, Camera* pcam);
TriangleMesh* bface_cull(float* matrix, TriangleMesh* ptri);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* pmesh, int start_idx);

#endif
