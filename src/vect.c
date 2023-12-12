#ifndef VECT_H
#define VECT_H

#include "primitives.h"

Point3D pt_add(Point3D a, Point3D b){
    Point3D res = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
    return res;
}

Point3D pt_diff(Point3D a, Point3D b){
    Point3D res = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
    return res;
}

Point3D pt_mul(float m, Point3D pt){
    Point3D res = {
        pt.x * m,
        pt.y * m,
        pt.z * m,
    };
    return res;
}

Point3D cross_product(Point3D a, Point3D b){
    Point3D res = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return res;
}

float dot_product(Point3D a, Point3D b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

#endif
