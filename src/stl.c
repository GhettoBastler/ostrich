#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vect.h"
#include "primitives.h"
#include "transforms.h"

// https://en.wikipedia.org/wiki/STL_(file_format)
typedef struct __attribute__((__packed__)) {
    float normal[3];
    float vertex1[3];
    float vertex2[3];
    float vertex3[3];
    int16_t attrib;
} STL_Triangle;

typedef struct __attribute__((__packed__)) {
    int32_t size;
    STL_Triangle triangles[];
} STL;

TriangleMesh* stl_to_tri_mesh(FILE* pfile){
    TriangleMesh* pres = malloc(sizeof(TriangleMesh));
    pres->size = 0;
    // Skip the first 80 bytes
    fseek(pfile, 80, SEEK_SET);

    // Read the number of triangles
    int32_t size;
    fread(&size, sizeof(int32_t), 1, pfile);
    printf("size: %d\n", size);

    // Read the triangle data
    STL_Triangle curr_stl_triangle;
    Triangle curr_triangle;

    for (int i = 0; i < size; i++){
        fread(&curr_stl_triangle, sizeof(STL_Triangle), 1, pfile);

        curr_triangle.b.x = curr_stl_triangle.vertex1[0];
        curr_triangle.b.y = curr_stl_triangle.vertex1[1];
        curr_triangle.b.z = curr_stl_triangle.vertex1[2];
        curr_triangle.a.x = curr_stl_triangle.vertex3[0];
        curr_triangle.a.y = curr_stl_triangle.vertex3[1];
        curr_triangle.a.z = curr_stl_triangle.vertex3[2];
        curr_triangle.c.x = curr_stl_triangle.vertex2[0];
        curr_triangle.c.y = curr_stl_triangle.vertex2[1];
        curr_triangle.c.z = curr_stl_triangle.vertex2[2];

        curr_triangle.visible[0] = true;
        curr_triangle.visible[1] = true;
        curr_triangle.visible[2] = true;

        pres = add_triangle(pres, curr_triangle);
    }
    printf("STL imported: %d triangles\n", pres->size);
    return pres;
}
