#define EPSILON 0.0005 // Arbitrary value to avoid lines intersecting with their own faces

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "camera.h"
#include "primitives.h"
#include "transforms.h"
#include "vect.h"
#include "utils.h"


Camera make_camera(float width, float height, float focal_length){
    Camera cam;
    cam.focal_length = focal_length;
    cam.width = width;
    cam.height = height;
    cam.orbit_radius = 0;

    for (int i = 0; i < 16; i++)
        cam.transform_mat[i] = 0;

    cam.transform_mat[0] = cam.transform_mat[5]
                         = cam.transform_mat[10]
                         = cam.transform_mat[15] = 1;

    return cam;
}

void update_transform_matrix(float* mat, Point3D rotation, Point3D translation, bool orbit, float orbit_radius){
    float new_mat[16],
          tmp_mat[16];
    if (orbit){
        Point3D z_translate = {0, 0, -orbit_radius};
        // Bringing the model to the camera
        calculate_translation_matrix(new_mat, z_translate);
        // Rotating it
        calculate_rotation_matrix(tmp_mat, rotation);
        multiply_matrix(new_mat, tmp_mat);
        // Putting it back
        calculate_translation_matrix(tmp_mat, pt_mul(-1.0, z_translate));
        multiply_matrix(new_mat, tmp_mat);
    } else {
        calculate_rotation_matrix(new_mat, rotation);
    }
    // Translating
    calculate_translation_matrix(tmp_mat, translation);
    multiply_matrix(new_mat, tmp_mat);
    multiply_matrix(mat, new_mat);
}
