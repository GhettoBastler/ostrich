#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <stdbool.h>

// STRUCTS
// 2D
typedef struct {
    float x, y;
} Point2D;

typedef struct {
    Point2D a, b;
} Edge2D;

typedef struct {
    int size;
    Edge2D edges[];
} Mesh2D;

// 3D
typedef struct {
   float x, y, z; 
} Point3D;

typedef struct {
    Point3D a, b;
} Edge3D;

typedef struct {
    int size;
    Edge3D edges[];
} Mesh3D;

// For hidden lines
typedef struct {
    Edge2D edge2D;
    Edge3D edge3D;
} ProjectedEdge;


// TRIANGULATED MESH
typedef struct {
    Point3D a, b, c;
    bool visible[3]; // Tell if AB/BC/CD is visible or not
} Triangle;

typedef struct {
    int size;
    Triangle triangles[];
} TriangleMesh;

// PROJECTED EDGES
typedef struct {
    int size;
    ProjectedEdge edges[];
} ProjectedMesh;

TriangleMesh* tri_cube(float size);

#endif
