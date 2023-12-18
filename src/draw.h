#ifndef DRAW_H
#define DRAW_H

#define LINE_COLOR_1 0xFFF4115D
#define LINE_COLOR_2 0xFF0296F2
#define BG_COLOR 0xFF111111

#include <stdint.h>
#include <stdbool.h>
#include "primitives.h"
#include "camera.h"

void draw_line(uint32_t* ppixels, ProjectedEdge edge, TriangleMesh* pmesh, bool draw_hidden, Camera* pcam);

#endif
