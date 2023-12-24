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

// Polygons
typedef struct _pv{
    int index;
    Point2D coordinates;
    struct _pv* prev;
    struct _pv* next;
} PolygonVertex;

typedef struct {
    int size;
    PolygonVertex* head;
} Polygon;

// 3D
typedef struct {
   float x, y, z; 
} Point3D;

typedef struct {
    Point3D a, b;
} Edge3D;

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


// FUNCTIONS
TriangleMesh* new_triangle_mesh();
TriangleMesh* tri_cube(float size);
Polygon* new_polygon(Point2D* vertices, int size);
Polygon* new_regular_polygon(float radius, int n_sides);
void free_polygon(Polygon* ppoly);
TriangleMesh* triangulate(Polygon* ppoly);
TriangleMesh* triangulated_regular_polygon(float radius, int n_sides);

#endif
