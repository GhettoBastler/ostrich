#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <stdio.h>
#include <math.h>
#include "primitives.h"

TriangleMesh* add_triangle(TriangleMesh* pmesh, Triangle tri);
TriangleMesh* merge_tri_meshes(TriangleMesh* pmesh1, TriangleMesh* pmesh2);

#endif
