#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "utils.h"
#include "camera.h"

void check_allocation(void* pointer, char* message){
    if (pointer == NULL){
        fprintf(stderr, message);
        exit(1);
    }
}

float deg_to_rad(float deg){
    return (deg / 180) * M_PI;
}

Point2D project_point(Point3D point, Camera* pcam){
    float x = point.x * (pcam->focal_length / (point.z));
    float y = point.y * (pcam->focal_length / (point.z));
    Point2D res = {x, y};
    return res;
}

