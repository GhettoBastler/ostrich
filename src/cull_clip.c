#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "cull_clip.h"
#include "camera.h"
#include "vect.h"
#include "transforms.h"


int comp_tri_z(const void* ptri_a, const void* ptri_b);
void clip_line(Edge3D* pedge, float ratio, bool reverse);
bool facing_camera(Triangle tri);
bool ray_tri_intersect(Point3D* inter, Point3D point, Triangle tri);


void clip_line(Edge3D* pedge, float ratio, bool reverse){
    if (reverse)
        pedge->a = pt_add(pedge->b, pt_mul(ratio, pt_diff(pedge->a, pedge->b)));
    else
        pedge->b = pt_add(pedge->a, pt_mul(ratio, pt_diff(pedge->b, pedge->a)));
}

void clip_frustum(Edge3D* pedge, Camera* pcam){
    Point3D diff;
    float ratio;

    // Clip edges that go behind the focal plane
    bool a_hidden = pedge->a.z < pcam->focal_length,
         b_hidden = pedge->b.z < pcam->focal_length;

    if (a_hidden && b_hidden){
        // Both points are hidden
        // We set the edge's coordinates to zero 
        pedge->a.x = pedge->a.y = pedge->a.z =
        pedge->b.x = pedge->b.y = pedge->b.z = 0;
        return;

    } else {
        // One of the point is visible
        if (a_hidden) {
            // A is hidden, B is visible
            ratio = (pcam->focal_length - pedge->b.z)/(pedge->a.z - pedge->b.z);
            clip_line(pedge, ratio, true);
        } else if (b_hidden){
            // B is hidden, A is visible
            ratio = (pcam->focal_length - pedge->a.z)/(pedge->b.z - pedge->a.z);
            clip_line(pedge, ratio, false);
        }
        // Both point are visible, we don't clip
    }
    
    // Clip edges around
    Point2D a_proj = project_point(pedge->a, pcam),
            b_proj = project_point(pedge->b, pcam);

    diff = pt_diff(pedge->b, pedge->a);
    float dx_proj = b_proj.x - a_proj.x,
          dy_proj = b_proj.y - a_proj.y;

    // Both A and B are outside the frustum
    if ((a_proj.x < -pcam->width/2 && b_proj.x < -pcam->width/2) ||
        (a_proj.y < -pcam->height/2 && b_proj.y < -pcam->height/2) ||
        (a_proj.x > pcam->width/2 && b_proj.x > pcam->width/2) ||
        (a_proj.y > pcam->height/2 && b_proj.y > pcam->height/2)){
        pedge->a.x = pedge->a.y = pedge->a.z =
        pedge->b.x = pedge->b.y = pedge->b.z = 0; // Edge is null
        return;

    } 
    // Moving A
    // X axis
    ratio = 1;
    if (diff.x != 0 && abs(a_proj.x) > abs(pcam->width/2)){
        if (a_proj.x < 0){ // A is too far to the left
            ratio = 1 + ((a_proj.x + pcam->width/2) / dx_proj);
        } else { // A is too far to the right
            ratio = 1 + ((a_proj.x - pcam->width/2) / dx_proj);
        }
    }
    // Y axis
    if (diff.y != 0 && abs(a_proj.y) > abs(pcam->height/2)){
        if (a_proj.y < 0){ // A is too far up
            ratio = fminf(ratio, 1 + ((a_proj.y + pcam->height/2) / dy_proj));
        } else { // A is too far down
            ratio = fminf(ratio, 1 + ((a_proj.y - pcam->height/2) / dy_proj));
        }
    }
    if (ratio > 0 && ratio < 1){
        clip_line(pedge, ratio, true);
        // Recalculating difference with the new A coordinates
        diff = pt_diff(pedge->b, pedge->a);
        a_proj = project_point(pedge->a, pcam); // projecting new A
        dx_proj = a_proj.x - b_proj.x;
        dy_proj = a_proj.y - b_proj.y;
    }

    // Moving B
    // X axis
    ratio = 1;
    if (diff.x != 0 && abs(b_proj.x) > abs(pcam->width/2)){
        if (b_proj.x < 0){ // B is too far to the left
            ratio = 1 + ((b_proj.x + pcam->width/2) / dx_proj);
        } else { // B is too far to the right
            ratio = 1 + ((b_proj.x - pcam->width/2) / dx_proj);
        }
    }
    // Y axis
    if (diff.y != 0 && abs(b_proj.y) > abs(pcam->height/2)){
        if (b_proj.y < 0){ // B is too far up
            ratio = fminf(ratio, 1 - ((b_proj.y + pcam->height/2) / dy_proj));
        } else { // B is too far down
            ratio = fminf(ratio, (pcam->height/2 - a_proj.y) / dy_proj);
        }
    }
    if (ratio > 0 && ratio < 1){
        clip_line(pedge, ratio, false);
    }
}

bool facing_camera(Triangle tri){
    Point3D vect_1 = pt_diff(tri.b, tri.a),
            vect_2 = pt_diff(tri.a, tri.c);
    Point3D normal = cross_product(vect_1, vect_2);
    Point3D center = pt_mul((float)1/3, pt_add(pt_add(tri.a, tri.b), tri.c));
    return (dot_product(center, normal) >= 0);
}

int comp_tri_z(const void* ptri_a, const void* ptri_b){
    Point3D tri_a_a = ((Triangle*) ptri_a)->a,
            tri_a_b = ((Triangle*) ptri_a)->b,
            tri_a_c = ((Triangle*) ptri_a)->c,
            tri_b_a = ((Triangle*) ptri_b)->a,
            tri_b_b = ((Triangle*) ptri_b)->b,
            tri_b_c = ((Triangle*) ptri_b)->c;

    float min_z_a = fminf(fminf(tri_a_a.z, tri_a_b.z), tri_a_c.z),
          min_z_b = fminf(fminf(tri_b_a.z, tri_b_b.z), tri_b_c.z);

    if (min_z_a < min_z_b)
        return -1;
    else if (min_z_a == min_z_b)
        return 0;
    else
        return 1;
}

TriangleMesh* bface_cull(float* matrix, TriangleMesh* ptri){
    TriangleMesh* pres = (TriangleMesh*) malloc(sizeof(TriangleMesh));
    pres->size = 0;
    Point3D trans_a, trans_b, trans_c;
    Triangle trans_tri;
    Edge3D ab, bc, ca;

    for (int i = 0; i < ptri->size; i++){
        trans_a = transform_point(matrix, ptri->triangles[i].a);
        trans_b = transform_point(matrix, ptri->triangles[i].b);
        trans_c = transform_point(matrix, ptri->triangles[i].c);

        trans_tri.a = trans_a;
        trans_tri.b = trans_b;
        trans_tri.c = trans_c;
        memcpy(trans_tri.visible, ptri->triangles[i].visible, 3);

        if (facing_camera(trans_tri)){
            pres = add_triangle(pres, trans_tri);
        }
    }

    // Z-sort triangles
    qsort(pres->triangles, pres->size, sizeof(Triangle), comp_tri_z);
    return pres;
}

bool ray_tri_intersect(Point3D* inter, Point3D point, Triangle tri){
    Point3D ab = pt_diff(tri.b, tri.a),
            bc = pt_diff(tri.c, tri.b),
            ca = pt_diff(tri.a, tri.c),
            normal = cross_product(ab, bc);

    float q = dot_product(normal, tri.a) / dot_product(normal, point);

    *inter = pt_mul(q, point);

    return (cross_product(pt_diff(*inter, tri.a), ab).z > 0 &&
            cross_product(pt_diff(*inter, tri.b), bc).z > 0 &&
            cross_product(pt_diff(*inter, tri.c), ca).z > 0);
}

bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* ptri_mesh, int start_idx){
    Point3D pt_obj = pt_add(pt_mul(ratio, edge.b),
                            pt_mul((1-ratio), edge.a));

    Triangle curr_tri;
    Point3D intersect;
    Point3D bbox_min, bbox_max;

    for (int i = start_idx; i<ptri_mesh->size; i++){
        curr_tri = ptri_mesh->triangles[i];
        bbox_min = pt_min(pt_min(curr_tri.a, curr_tri.b), curr_tri.c);
        bbox_max = pt_max(pt_max(curr_tri.a, curr_tri.b), curr_tri.c);

        // If point is totally in front of curr_tri, it doesn't intersect it
        if (pt_obj.z < bbox_min.z)
            return true;
        
        // If point is projected outside of the triangle bounding box, it doesn't hide it
        if (
           (
            (pt_obj.x * bbox_max.z / pt_obj.z < bbox_min.x) &&
            (pt_obj.x * bbox_min.z / pt_obj.z < bbox_min.x)
           ) || (
            (pt_obj.x * bbox_max.z / pt_obj.z > bbox_max.x) &&
            (pt_obj.x * bbox_min.z / pt_obj.z > bbox_max.x)
           ) || (
            (pt_obj.y * bbox_max.z / pt_obj.z < bbox_min.y) &&
            (pt_obj.y * bbox_min.z / pt_obj.z < bbox_min.y)
           ) || (
            (pt_obj.y * bbox_max.z / pt_obj.z > bbox_max.y) &&
            (pt_obj.y * bbox_min.z / pt_obj.z > bbox_max.y)
           )
        )
            continue;

        if (ray_tri_intersect(&intersect, pt_obj, curr_tri)){
            if (intersect.z + 0.0001 < pt_obj.z){ // Adding an arbitrary value to make sure that edges don't intersect their own faces
                return false;
            }
        }
    }
    return true;
}
