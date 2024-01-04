#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "utils.h"

void check_allocation(void* pointer, char* message){
    if (pointer == NULL){
        fprintf(stderr, message);
        exit(1);
    }
}

float deg_to_rad(float deg){
    return (deg / 180) * M_PI;
}

