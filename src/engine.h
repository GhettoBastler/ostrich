#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

typedef struct {
    bool reproject,
         orbit,
         hlr,
         do_hlr,
         bface_cull;
} EngineState;

#endif
