#include <stdlib.h>
#include <time.h>
#include "vect.h"
#include "transforms.h"
#include "primitives.h"
#include "interpreter.h"

TriangleMesh* tri_make_scene(){
    TriangleMesh* pscene = parse_file();
    return pscene;
}
