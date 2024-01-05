#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "camera.h"
#include "cull_clip.h"
#include "transforms.h"
#include "vect.h"

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

// 2D projection
Point2D project_point(Point3D point, Camera* pcam){
    float x = point.x * (pcam->focal_length / (point.z));
    float y = point.y * (pcam->focal_length / (point.z));
    Point2D res = {x, y};
    return res;
}

ProjectedEdge project_edge(Edge3D edge, Camera* pcam){
    ProjectedEdge res;
    Point2D a = project_point(edge.a, pcam);
    Point2D b = project_point(edge.b, pcam);
    res.edge3D = edge;
    res.edge2D.a = a;
    res.edge2D.b = b;

    return res;
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

TriangleMesh* project_tri_mesh(ProjectedMesh* pbuffer, TriangleMesh* ptri_mesh, Camera* pcam, bool do_cull){
    // Creating a copy of the mesh for transform
    TriangleMesh* pmesh_transformed = copy_mesh(ptri_mesh);
    // 3D transform
    transform_mesh(pcam->transform_mat, pmesh_transformed);
    // Culling
    TriangleMesh* pculled_tri;
    TriangleMesh* pfrustum_culled;
    // Frustum culling (always)
    pfrustum_culled = frustum_cull(pmesh_transformed, pcam);
    if (do_cull){
        pculled_tri = bface_cull(pfrustum_culled);
        free(pfrustum_culled);
    } else {
        pculled_tri = pfrustum_culled;
    }
    
    Edge3D edges[3];
    ProjectedEdge curr_proj_edge;

    int n = 0;
    for (int i = 0; i < pculled_tri->size; i++){
        // Convert each triangle into three edges
        // AB
        edges[0].a = pculled_tri->triangles[i].a;
        edges[0].b = pculled_tri->triangles[i].b;
        // BC
        edges[1].a = pculled_tri->triangles[i].b;
        edges[1].b = pculled_tri->triangles[i].c;
        // CA
        edges[2].a = pculled_tri->triangles[i].c;
        edges[2].b = pculled_tri->triangles[i].a;

        for (int j = 0; j < 3; j++){
            // Add only the visible edges
            if (pculled_tri->triangles[i].visible[j]) {
                // Clip the line if it goes outside the frustum
                clip_frustum(&edges[j], pcam);
                // If there is nothing left
                if (pt_is_null(edges[j].a) && pt_is_null(edges[j].b))
                    continue;
                // Project the edge
                curr_proj_edge = project_edge(edges[j], pcam);
                // Add it to the mesh
                pbuffer->edges[n] = curr_proj_edge;
                n += 1;
            }
        }
    }
    pbuffer->size = n;
    // Z-sort
    z_sort_triangles(pculled_tri);
    return pculled_tri;
}
