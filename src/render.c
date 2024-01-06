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
#include "render.h"

Point2D project_point(Point3D point, Camera* pcam);
ProjectedEdge project_edge(Edge3D edge, Camera* pcam);
TriangleMesh* project_tri_mesh(ProjectedMesh* pbuffer, TriangleMesh* ptri_mesh, Camera* pcam, bool do_cull);
void clip_frustum(Edge3D* pedge, Camera* pcam);
bool ray_tri_intersect(Point3D* inter, Point3D point, Triangle tri);
bool point_is_visible(Edge3D edge, float ratio, TriangleMesh* ptri_mesh, int start_idx);
float obj_ratio_from_screen_ratio(Edge3D edge3D, Edge2D edge2D, float focal_length,
                                  float ratio, bool reverse);
void draw_line(uint32_t* ppixels, ProjectedEdge edge, TriangleMesh* pmesh,
               bool draw_hidden, Camera* pcam);
void clip_line(Edge3D* pedge, float ratio, bool reverse);

void render_mesh(TriangleMesh* pmesh, uint32_t* ppixels, Camera* pcam, bool do_bface_cull, bool do_hlr){
    for (int i = 0; i < HEIGHT * WIDTH; i++){
        ppixels[i] = BG_COLOR;
    }
    ProjectedMesh* pproj = new_projected_mesh(pmesh->size);
    TriangleMesh* pculled = project_tri_mesh(pproj, pmesh, pcam, do_bface_cull);
    for (int i = 0; i < pproj->size; i++){
        draw_line(ppixels, pproj->edges[i], pculled, !do_hlr, pcam);
    }
    free(pculled);
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
        obj_ratio = obj_ratio_from_screen_ratio(*pedge, edge_proj, pcam->focal_length,
                                                ratio, true);
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
        obj_ratio = obj_ratio_from_screen_ratio(*pedge, edge_proj, pcam->focal_length,
                                                ratio, false);
        clip_line(pedge, obj_ratio, false);
    }
}

bool ray_tri_intersect(Point3D* inter, Point3D point, Triangle tri){
    Point3D ab = pt_diff(tri.b, tri.a),
            bc = pt_diff(tri.c, tri.b),
            ca = pt_diff(tri.a, tri.c),
            normal = cross_product(ab, bc);

    float q = dot_product(normal, tri.a) / dot_product(normal, point);

    *inter = pt_mul(q, point);

    // Is the ray intersecting with a triangle behind the camera ?
    if (inter->z < 0)
        return false;

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

        if (ray_tri_intersect(&intersect, pt_obj, curr_tri)){
            if (intersect.z + EPSILON < pt_obj.z){
                return false;
            }
        }
    }
    return true;
}

float obj_ratio_from_screen_ratio(Edge3D edge3D, Edge2D edge2D, float focal_length,
                                  float ratio, bool reverse){
    float res;
    if (reverse){
        Point3D tmp3 = edge3D.a;
        edge3D.a = edge3D.b;
        edge3D.b = tmp3;
        Point2D tmp2 = edge2D.a;
        edge2D.a = edge2D.b;
        edge2D.b = tmp2;
    }
    
    // Convert to ratio in object space
    // Check denominators to avoid infinitely large ratios 
    float denom_x = (
        (
            edge3D.b.x - edge3D.a.x
        ) * focal_length - (
            edge3D.b.z - edge3D.a.z
        ) * (
            edge2D.a.x + ratio * (
                edge2D.b.x - edge2D.a.x
            )
        )
    ),
          denom_y = (
        (
            edge3D.b.y - edge3D.a.y
        ) * focal_length - (
            edge3D.b.z - edge3D.a.z
        ) * (
            edge2D.a.y + ratio * (
                edge2D.b.y - edge2D.a.y
            )
        )
    );
    if (denom_x != 0)
        // Line is horizontal use denom_x
        res = (
                edge3D.a.z * (
                    edge2D.a.x + ratio * (
                        edge2D.b.x - edge2D.a.x
                    )
                ) - edge3D.a.x * focal_length
            )/denom_x;
    else if (denom_y != 0)
        // Line is vertical use denom_y
        res = (
                edge3D.a.z * (
                    edge2D.a.y + ratio * (
                        edge2D.b.y - edge2D.a.y
                    )
                ) - edge3D.a.y * focal_length
            )/denom_y;
    else
        // Line is a point
        res = 0;

    return res;
}

void draw_line(uint32_t* ppixels, ProjectedEdge edge, TriangleMesh* pmesh,
               bool draw_hidden, Camera* pcam){

    Edge2D centered = edge.edge2D;
    centered.a.x = (centered.a.x + pcam->width/2)*SCALE;
    centered.a.y = (centered.a.y + pcam->height/2)*SCALE;
    centered.b.x = (centered.b.x + pcam->width/2)*SCALE;
    centered.b.y = (centered.b.y + pcam->height/2)*SCALE;

    int x0 = (int) centered.a.x,
        y0 = (int) centered.a.y,
        x1 = (int) centered.b.x,
        y1 = (int) centered.b.y;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx+dy,
        e2;

    float screen_ratio, obj_ratio;
    float span = sqrt(pow(dx, 2) + pow(dy, 2));
    int x_init = x0;
    int y_init = y0;

    int i=0;

    if (!draw_hidden){
        // Check when the line's bounding box gets obstructed
        Triangle curr_tri;

        BoundingBox edge_bbox = bbox_from_edge(edge.edge3D),
                    tri_bbox;

        for (i=0; i < pmesh->size; i++){
            curr_tri = pmesh->triangles[i];

            tri_bbox = bbox_from_triangle(curr_tri);

            // If the edge's bbox is completely in front of this triangle's bounding box,
            // it is not hidden by this triangle, nor by any other
            if (edge_bbox.max.z < tri_bbox.min.z){
                i = pmesh->size - 1;
                break;
            }
            break;
        }
    };

    // Bresenham
    for (;;){
        screen_ratio = sqrt(pow(x0 - x_init, 2) + pow(y0 - y_init, 2)) / span;
        // Convert to ratio in object space
        obj_ratio = obj_ratio_from_screen_ratio(edge.edge3D, edge.edge2D,
                                                pcam->focal_length, screen_ratio, false);
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
            if (draw_hidden)
                ppixels[x0 + WIDTH * y0] = LINE_COLOR_1;
            else if (point_is_visible(edge.edge3D, obj_ratio, pmesh, i))
                    ppixels[x0 + WIDTH * y0] = LINE_COLOR_2;

        if (x0 == x1 && y0 == y1) break;
        e2 = 2*err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}
