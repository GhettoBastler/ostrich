#ifndef DRAW_H
#define DRAW_H

#define LINE_COLOR_1 0xFFF4115D
#define LINE_COLOR_2 0xFF000000
#define BG_COLOR 0xFFFFFFFF

#include <stdint.h>
#include <stdbool.h>
#include "primitives.h"
#include "camera.h"

void draw_line(uint32_t* ppixels, ProjectedEdge edge, TriangleMesh* pmesh, bool draw_hidden, Camera* pcam);

#endif
