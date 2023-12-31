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
TriangleMesh* transform_mesh(float* matrix, TriangleMesh* pmesh); 
Triangle transform_triangle(float* matrix, Triangle tri);
TriangleMesh* transform_mesh(float* matrix, TriangleMesh* pmesh);
void calculate_rotation_matrix(float* matrix, Point3D rotation);
void calculate_translation_matrix(float* matrix, Point3D translation);
void translate_mesh(TriangleMesh* pmesh, Point3D rotation);

#endif
