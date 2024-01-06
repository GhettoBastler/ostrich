#ifndef VECT_H
#define VECT_H

#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "primitives.h"

float pt_len(Point3D a);
bool pt_equ(Point3D a, Point3D b);
Point3D pt_add(Point3D a, Point3D b);
Point3D pt_diff(Point3D a, Point3D b);
Point3D pt_mul(float m, Point3D pt);
bool pt_is_null(Point3D pt);
Point3D cross_product(Point3D a, Point3D b);
float dot_product(Point3D a, Point3D b);
Point3D pt_min(Point3D a, Point3D b);
Point3D pt_max(Point3D a, Point3D b);
void multiply_matrix(float* ma, float* mb);
void check_allocation(void* pointer, char* message);
Point3D normalize(Point3D vect);

#endif
