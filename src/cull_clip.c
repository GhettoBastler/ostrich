#define EPSILON 0.0005

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
bool point_in_bbox(Point3D point, BoundingBox bbox);
float obj_ratio_from_screen_ratio(Edge3D edge3D, Edge2D edge2D, float focal_length, float ratio, bool reverse);


void clip_line(Edge3D* pedge, float ratio, bool reverse){
    if (reverse)
        pedge->a = pt_add(pedge->b, pt_mul(ratio, pt_diff(pedge->a, pedge->b)));
    else
        pedge->b = pt_add(pedge->a, pt_mul(ratio, pt_diff(pedge->b, pedge->a)));
}

void clip_frustum(Edge3D* pedge, Camera* pcam){
    Point3D diff;
    float ratio, obj_ratio;

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
    Edge2D edge_proj = {a_proj, b_proj};

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
        obj_ratio = obj_ratio_from_screen_ratio(*pedge, edge_proj, pcam->focal_length, ratio, true);
        // clip_line(pedge, ratio, true);
        clip_line(pedge, obj_ratio, true);
        // Recalculating difference with the new A coordinates
        diff = pt_diff(pedge->b, pedge->a);
        a_proj = project_point(pedge->a, pcam); // projecting new A
        dx_proj = a_proj.x - b_proj.x;
        dy_proj = a_proj.y - b_proj.y;
        edge_proj.a = a_proj;
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
        obj_ratio = obj_ratio_from_screen_ratio(*pedge, edge_proj, pcam->focal_length, ratio, false);
        // clip_line(pedge, ratio, false);
        clip_line(pedge, obj_ratio, false);
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
    Triangle curr_tri;
    pres->size = 0;

    for (int i = 0; i < ptri->size; i++){
        curr_tri = ptri->triangles[i];
        //if (facing_camera(ptri->triangles[i])){
            //pres = add_triangle(pres, ptri->triangles[i]);
        if (facing_camera(curr_tri)){
            pres = add_triangle(pres, curr_tri);
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

    return (
        (
            cross_product(pt_diff(*inter, tri.a), ab).z > 0 &&
            cross_product(pt_diff(*inter, tri.b), bc).z > 0 &&
            cross_product(pt_diff(*inter, tri.c), ca).z > 0
        ) || (
            cross_product(pt_diff(*inter, tri.a), ab).z <= 0 &&
            cross_product(pt_diff(*inter, tri.b), bc).z <= 0 &&
            cross_product(pt_diff(*inter, tri.c), ca).z <= 0
        )
    );
}

bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* ptri_mesh, int start_idx){
    Point3D pt_obj = pt_add(pt_mul(ratio, edge.b),
                            pt_mul((1-ratio), edge.a));

    Triangle curr_tri;
    Point3D intersect;
    BoundingBox bbox;

    for (int i = start_idx; i<ptri_mesh->size; i++){
        curr_tri = ptri_mesh->triangles[i];
        bbox = bbox_from_triangle(curr_tri);

        // If point is totally in front of curr_tri, it doesn't intersect it
        if (pt_obj.z < bbox.min.z)
            return true;
        
        // If point is projected outside of the triangle bounding box, it doesn't hide it
        // Point3D proj1 = pt_mul(bbox.max.z / pt_obj.z, pt_obj),
        //         proj2 = pt_mul(bbox.min.z / pt_obj.z, pt_obj);
        // if (!point_in_bbox(proj1, bbox) && !point_in_bbox(proj2, bbox))
        //     continue;

        if (ray_tri_intersect(&intersect, pt_obj, curr_tri)){
            if (intersect.z > 0){ // Is the ray intersecting with a triangle in front of the camera ?
                if (intersect.z + EPSILON < pt_obj.z){ // Adding an arbitrary value to make sure that edges don't intersect their own faces
                    return false;
                }
            }
        }
    }
    return true;
}

BoundingBox bbox_from_triangle(Triangle triangle){
    BoundingBox res;
    res.min = pt_min(pt_min(triangle.a, triangle.b), triangle.c);
    res.max = pt_max(pt_max(triangle.a, triangle.b), triangle.c);
    return res;
}

BoundingBox bbox_from_edge(Edge3D edge){
    BoundingBox res;
    res.min = pt_min(edge.a, edge.b);
    res.max = pt_max(edge.a, edge.b);
    return res;
}

bool point_in_bbox(Point3D point, BoundingBox bbox){
    return (
        (point.x >= bbox.min.x - EPSILON) && (point.x - EPSILON <= bbox.max.x) &&
        (point.y >= bbox.min.y - EPSILON) && (point.y - EPSILON <= bbox.max.y) &&
        (point.z >= bbox.min.z - EPSILON) && (point.z - EPSILON <= bbox.max.z));
}

bool bbox_in_shadow(BoundingBox covered, BoundingBox covering){
    // Dumb way: project every corner of the covering bounding
    // box on both the near and far plane of the covered bbox

    Point3D proj_A11, proj_B11, proj_C11, proj_D11,
            proj_A12, proj_B12, proj_C12, proj_D12,
            proj_A21, proj_B21, proj_C21, proj_D21,
            proj_A22, proj_B22, proj_C22, proj_D22,
            A1, B1, C1, D1,
            A2, B2, C2, D2;

    A1.x = A2.x = D1.x = D2.x = covering.min.x;
    A1.y = A2.y = B1.y = B2.y = covering.min.y;
    A1.z = B1.z = C1.z = D1.z = covering.min.z;
    B1.x = B2.x = C1.x = C2.x = covering.max.x;
    C1.y = C2.y = D1.y = D2.y = covering.max.y;
    A2.z = B2.z = C2.z = D2.z = covering.max.z;

    float ratio11 = covered.min.z / covering.min.z,
          ratio12 = covered.max.z / covering.min.z,
          ratio21 = covered.min.z / covering.max.z,
          ratio22 = covered.max.z / covering.max.z;

    proj_A11 = pt_mul(ratio11, A1);
    proj_B11 = pt_mul(ratio11, B1);
    proj_C11 = pt_mul(ratio11, C1);
    proj_D11 = pt_mul(ratio11, D1);
    proj_A12 = pt_mul(ratio12, A1);
    proj_B12 = pt_mul(ratio12, B1);
    proj_C12 = pt_mul(ratio12, C1);
    proj_D12 = pt_mul(ratio12, D1);
    proj_A21 = pt_mul(ratio21, A2);
    proj_B21 = pt_mul(ratio21, B2);
    proj_C21 = pt_mul(ratio21, C2);
    proj_D21 = pt_mul(ratio21, D2);
    proj_A22 = pt_mul(ratio22, A2);
    proj_B22 = pt_mul(ratio22, B2);
    proj_C22 = pt_mul(ratio22, C2);
    proj_D22 = pt_mul(ratio22, D2);

    // Then, we check if any of these points lies inside the covered bounding box
    return (point_in_bbox(proj_A11, covered) ||
            point_in_bbox(proj_B11, covered) ||
            point_in_bbox(proj_C11, covered) ||
            point_in_bbox(proj_D11, covered) ||
            point_in_bbox(proj_A21, covered) ||
            point_in_bbox(proj_B21, covered) ||
            point_in_bbox(proj_C21, covered) ||
            point_in_bbox(proj_D21, covered) ||
            point_in_bbox(proj_A12, covered) ||
            point_in_bbox(proj_B12, covered) ||
            point_in_bbox(proj_C12, covered) ||
            point_in_bbox(proj_D12, covered) ||
            point_in_bbox(proj_A22, covered) ||
            point_in_bbox(proj_B22, covered) ||
            point_in_bbox(proj_C22, covered) ||
            point_in_bbox(proj_D22, covered));
}

float obj_ratio_from_screen_ratio(Edge3D edge3D, Edge2D edge2D, float focal_length, float ratio, bool reverse){
    if (reverse){
        Point3D tmp3 = edge3D.a;
        edge3D.a = edge3D.b;
        edge3D.b = tmp3;
        Point2D tmp2 = edge2D.a;
        edge2D.a = edge2D.b;
        edge2D.b = tmp2;
    }
    
    // Convert to ratio in object space
    float res = (edge3D.a.z * (edge2D.a.x + ratio * (edge2D.b.x - edge2D.a.x)) - edge3D.a.x * focal_length)/((edge3D.b.x - edge3D.a.x)*focal_length - (edge3D.b.z - edge3D.a.z)*(edge2D.a.x + ratio * (edge2D.b.x - edge2D.a.x)));

    return res;
}
