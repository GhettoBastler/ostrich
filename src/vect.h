#ifndef VECT_H
#define VECT_H

#include <stdbool.h>
#include "primitives.h"

bool pt_equ(Point3D a, Point3D b);
Point3D pt_add(Point3D a, Point3D b);
Point3D pt_diff(Point3D a, Point3D b);
Point3D pt_mul(float m, Point3D pt);
Point3D cross_product(Point3D a, Point3D b);
float dot_product(Point3D a, Point3D b);

#endif
