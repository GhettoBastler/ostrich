#ifndef PRIMITIVES_H
#define PRIMITIVES_H

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

// 3D MESHES
Mesh3D* prism(Mesh3D* pmesh, Point3D vect);
Mesh3D* box(float a, float b, float c);
Mesh3D* polygon(float radius, int n);
Mesh3D* line(float xa, float ya, float za, float xb, float yb, float zb);
Mesh3D* sphere(float radius);
Mesh3D* grid(float width, float length, int n_div_x, int n_div_y);

#endif
