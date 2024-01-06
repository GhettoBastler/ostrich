#include "vect.h"

bool pt_equ(Point3D a, Point3D b){
    return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
}

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

Point3D pt_min(Point3D a, Point3D b){
    Point3D res = {
        fminf(a.x, b.x),
        fminf(a.y, b.y),
        fminf(a.z, b.z)
    };
    return res;
}

Point3D pt_max(Point3D a, Point3D b){
    Point3D res = {
        fmaxf(a.x, b.x),
        fmaxf(a.y, b.y),
        fmaxf(a.z, b.z)
    };
    return res;
}

void multiply_matrix(float* matB, float* matA){
    float res[16];
    res[0] = matA[0] * matB[0] + matA[1] * matB[4] + matA[2] * matB[8] + matA[3] * matB[12];
    res[1] = matA[0] * matB[1] + matA[1] * matB[5] + matA[2] * matB[9] + matA[3] * matB[13];
    res[2] = matA[0] * matB[2] + matA[1] * matB[6] + matA[2] * matB[10] + matA[3] * matB[14];
    res[3] = matA[0] * matB[3] + matA[1] * matB[7] + matA[2] * matB[11] + matA[3] * matB[15];
    res[4] = matA[4] * matB[0] + matA[5] * matB[4] + matA[6] * matB[8] + matA[7] * matB[12];
    res[5] = matA[4] * matB[1] + matA[5] * matB[5] + matA[6] * matB[9] + matA[7] * matB[13];
    res[6] = matA[4] * matB[2] + matA[5] * matB[6] + matA[6] * matB[10] + matA[7] * matB[14];
    res[7] = matA[4] * matB[3] + matA[5] * matB[7] + matA[6] * matB[11] + matA[7] * matB[15];
    res[8] = matA[8] * matB[0] + matA[9] * matB[4] + matA[10] * matB[8] + matA[11] * matB[12];
    res[9] = matA[8] * matB[1] + matA[9] * matB[5] + matA[10] * matB[9] + matA[11] * matB[13];
    res[10] = matA[8] * matB[2] + matA[9] * matB[6] + matA[10] * matB[10] + matA[11] * matB[14];
    res[11] = matA[8] * matB[3] + matA[9] * matB[7] + matA[10] * matB[11] + matA[11] * matB[15];
    res[12] = matA[12] * matB[0] + matA[13] * matB[4] + matA[14] * matB[8] + matA[15] * matB[12];
    res[13] = matA[12] * matB[1] + matA[13] * matB[5] + matA[14] * matB[9] + matA[15] * matB[13];
    res[14] = matA[12] * matB[2] + matA[13] * matB[6] + matA[14] * matB[10] + matA[15] * matB[14];
    res[15] = matA[12] * matB[3] + matA[13] * matB[7] + matA[14] * matB[11] + matA[15] * matB[15];

    memcpy(matB, res, sizeof(float) * 16);
}

bool pt_is_null(Point3D pt){
    return (pt.x == 0 && pt.y == 0 && pt.z == 0);
}

float pt_len(Point3D pt){
    return sqrtf(dot_product(pt, pt));
}

Point3D normalize(Point3D vect){
    return pt_mul(1/pt_len(vect), vect);
}
