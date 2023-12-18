#include <math.h>
#include <stdlib.h>
#include "draw.h"
#include "vect.h"

void draw_line(uint32_t* ppixels, ProjectedEdge edge, TriangleMesh* pmesh, bool draw_hidden, Camera* pcam){

    Edge2D capped = edge.edge2D;
    capped.a.x = (capped.a.x + pcam->width/2)*SCALE;
    capped.a.y = (capped.a.y + pcam->height/2)*SCALE;
    capped.b.x = (capped.b.x + pcam->width/2)*SCALE;
    capped.b.y = (capped.b.y + pcam->height/2)*SCALE;

    int x0 = (int) capped.a.x,
        y0 = (int) capped.a.y,
        x1 = (int) capped.b.x,
        y1 = (int) capped.b.y;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx+dy,
        e2;

    float ratio;
    float span = sqrt(pow(dx, 2) + pow(dy, 2));
    int x_init = x0;
    int y_init = y0;

    int i=0;

    if (!draw_hidden){
        // Check when the line's bounding box gets obstructed
        Point3D tri_bbox_min, tri_bbox_max,
                edge_bbox_min = pt_min(edge.edge3D.a, edge.edge3D.b),
                edge_bbox_max = pt_max(edge.edge3D.a, edge.edge3D.b);
        Triangle curr_tri;

        for (i=0; i < pmesh->size; i++){
            curr_tri = pmesh->triangles[i];

            tri_bbox_min = pt_min(pt_min(curr_tri.a, curr_tri.b), curr_tri.c);
            tri_bbox_max = pt_max(pt_max(curr_tri.a, curr_tri.b), curr_tri.c);

            // If the edge's bbox is completely in front of this triangle's bounding box,
            // it is not hidden by this triangle, nor by any other
            if (edge_bbox_max.z < tri_bbox_min.z){
                i = pmesh->size - 1;
                break;
            }

            // If edge's bbox is projected completely outside of the triangle bounding box,
            // the triangle doesn't hide it
            if (
                (
                 (edge_bbox_max.x * tri_bbox_min.z / edge_bbox_max.z < tri_bbox_min.x) &&
                 (edge_bbox_max.x * tri_bbox_max.z / edge_bbox_max.z < tri_bbox_min.x)
                ) || (
                 (edge_bbox_min.x * tri_bbox_min.z / edge_bbox_max.z > tri_bbox_max.x) &&
                 (edge_bbox_min.x * tri_bbox_max.z / edge_bbox_max.z > tri_bbox_max.x)
                ) || (
                 (edge_bbox_max.y * tri_bbox_min.z / edge_bbox_max.z < tri_bbox_min.y) &&
                 (edge_bbox_max.y * tri_bbox_max.z / edge_bbox_max.z < tri_bbox_min.y)
                ) || (
                 (edge_bbox_min.y * tri_bbox_min.z / edge_bbox_max.z > tri_bbox_max.y) &&
                 (edge_bbox_min.y * tri_bbox_max.z / edge_bbox_max.z > tri_bbox_max.y)
                )
            )
                continue;

            // At this point, we know that the edge's bounding box is in the shadow of
            // this triangle's bounding box. Then we do the modified Bresenham with
            // the triangle list starting from this point
            break;
        }
    };

    // Bresenham
    for (;;){
        ratio = sqrt(pow(x0 - x_init, 2) + pow(y0 - y_init, 2)) / span;
        if (x0 >= 0 && x0 < WIDTH && y0 >= 0 && y0 < HEIGHT)
            if (draw_hidden)
                ppixels[x0 + WIDTH * y0] = LINE_COLOR_1;
            else if (point_is_visible(edge.edge3D, ratio, pmesh, i))
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
