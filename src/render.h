#ifndef RENDER_H
#define RENDER_H

#define LINE_COLOR_1 0xFFF4115D
#define LINE_COLOR_2 0xFF000000
#define BG_COLOR 0xFFFFFFFF

#include <stdint.h>

void render_mesh(TriangleMesh* pmesh, uint32_t* ppixels, Camera* pcam, bool do_bface_cull, bool do_hlr);

#endif
