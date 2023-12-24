#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <stdio.h>
#include <math.h>
#include "primitives.h"

TriangleMesh* add_triangle(TriangleMesh* pmesh, Triangle tri);
TriangleMesh* merge_tri_meshes(TriangleMesh* pmesh1, TriangleMesh* pmesh2);
void flip_triangle(Triangle* ptri);
void flip_mesh(TriangleMesh* pmesh);
TriangleMesh* extrude(Polygon* ppoly, float height);

#endif
