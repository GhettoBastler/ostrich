#ifndef UTILS_H
#define UTILS_H

#include "primitives.h"
#include "camera.h"

float deg_to_rad(float deg);
void check_allocation(void* pointer, char* message);
Point2D project_point(Point3D point, Camera* pcam);

#endif
