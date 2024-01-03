#include <math.h>
#include <stdlib.h>
#include "draw.h"
#include "vect.h"
#include "cull_clip.h"

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

            // If edge's bbox is projected completely outside of the triangle bounding box,
            // the triangle doesn't hide it
            if (!bbox_in_shadow(tri_bbox, edge_bbox))
                continue;

            // At this point, we know that the edge's bounding box is in the shadow of
            // this triangle's bounding box. Then we do the modified Bresenham with
            // the triangle list starting from this point
            break;
        }
    };

    // Bresenham
    for (;;){
        screen_ratio = sqrt(pow(x0 - x_init, 2) + pow(y0 - y_init, 2)) / span;
        // Convert to ratio in object space
        obj_ratio = (edge.edge3D.a.z * (edge.edge2D.a.x + screen_ratio * (edge.edge2D.b.x - edge.edge2D.a.x)) - edge.edge3D.a.x * pcam->focal_length)/((edge.edge3D.b.x - edge.edge3D.a.x)*pcam->focal_length - (edge.edge3D.b.z - edge.edge3D.a.z)*(edge.edge2D.a.x + screen_ratio * (edge.edge2D.b.x - edge.edge2D.a.x)));
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
