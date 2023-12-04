#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <stdio.h>
#include <math.h>
#include "primitives.h"

Mesh3D* add_edge(Mesh3D* pmesh, Edge3D edge);
Mesh3D* merge_meshes(Mesh3D* pmesh1, Mesh3D* pmesh2);
void rotate(Mesh3D* pmesh, Point3D r);
void translate(Mesh3D* pmesh, Point3D v);

#endif
