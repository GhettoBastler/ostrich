#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "primitives.h"
#include "transforms.h"

TriangleMesh* tri_cube(float size){
    TriangleMesh* pres = (TriangleMesh*) malloc(sizeof(TriangleMesh) + 12 * sizeof(Triangle));
    pres->size = 12;

    Point3D a = {0, 0, 0};
    Point3D b = {size, 0, 0};
    Point3D c = {size, size, 0};
    Point3D d = {0, size, 0};
    Point3D e = {0, 0, size};
    Point3D f = {size, 0, size};
    Point3D g = {size, size, size};
    Point3D h = {0, size, size};

    Triangle abc = {a, b, c};
    Triangle cda = {c, d, a};

    Triangle bfg = {b, f, g};
    Triangle gcb = {g, c, b};

    Triangle cgh = {c, g, h};
    Triangle hdc = {h, d, c};

    Triangle ead = {e, a, d};
    Triangle dhe = {d, h, e};

    Triangle feh = {f, e, h};
    Triangle hgf = {h, g, f};

    Triangle efb = {e, f, b};
    Triangle bae = {b, a, e};

    pres->triangles[0] = abc;
    pres->triangles[0].visible[0] = true;
    pres->triangles[0].visible[1] = true;
    pres->triangles[0].visible[2] = false;
    pres->triangles[1] = cda;
    pres->triangles[1].visible[0] = true;
    pres->triangles[1].visible[1] = true;
    pres->triangles[1].visible[2] = false;

    pres->triangles[2] = bfg;
    pres->triangles[2].visible[0] = true;
    pres->triangles[2].visible[1] = true;
    pres->triangles[2].visible[2] = false;
    pres->triangles[3] = gcb;
    pres->triangles[3].visible[0] = true;
    pres->triangles[3].visible[1] = true;
    pres->triangles[3].visible[2] = false;

    pres->triangles[4] = cgh;
    pres->triangles[4].visible[0] = true;
    pres->triangles[4].visible[1] = true;
    pres->triangles[4].visible[2] = false;
    pres->triangles[5] = hdc;
    pres->triangles[5].visible[0] = true;
    pres->triangles[5].visible[1] = true;
    pres->triangles[5].visible[2] = false;

    pres->triangles[6] = ead;
    pres->triangles[6].visible[0] = true;
    pres->triangles[6].visible[1] = true;
    pres->triangles[6].visible[2] = false;
    pres->triangles[7] = dhe;
    pres->triangles[7].visible[0] = true;
    pres->triangles[7].visible[1] = true;
    pres->triangles[7].visible[2] = false;

    pres->triangles[8] = feh;
    pres->triangles[8].visible[0] = true;
    pres->triangles[8].visible[1] = true;
    pres->triangles[8].visible[2] = false;
    pres->triangles[9] = hgf;
    pres->triangles[9].visible[0] = true;
    pres->triangles[9].visible[1] = true;
    pres->triangles[9].visible[2] = false;

    pres->triangles[10] = efb;
    pres->triangles[10].visible[0] = true;
    pres->triangles[10].visible[1] = true;
    pres->triangles[10].visible[2] = false;
    pres->triangles[11] = bae;
    pres->triangles[11].visible[0] = true;
    pres->triangles[11].visible[1] = true;
    pres->triangles[11].visible[2] = false;

    return pres;
}
