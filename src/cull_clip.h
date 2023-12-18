#ifndef CULL_CLIP_H
#define CULL_CLIP_H

#include "primitives.h"

TriangleMesh* bface_cull(float* matrix, TriangleMesh* ptri);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* pmesh, int start_idx);

#endif
