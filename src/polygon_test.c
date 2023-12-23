#include <stdio.h>
#include <stdlib.h>
#include "primitives.h"

void print_polygon(Polygon* ppoly);

int main(int argc, char** argv){
    Point2D vertices[6] = {
        {3, 6},
        {5, 5},
        {6, 4},
        {0, 0},
        {1, 1},
        {1, 4},
    };
    Polygon* ppoly = new_polygon(vertices, 6);
    print_polygon(ppoly);
    triangulate(ppoly);
    free_polygon(ppoly);
    return 0;
};


void print_polygon(Polygon* ppoly){
    int i = 0;
    PolygonVertex* pcurr = ppoly->head;
    while (pcurr->next != ppoly->head){
        printf("Vertex %d: %.2f %.2f\n", i, pcurr->coordinates.x, pcurr->coordinates.y);
        pcurr = pcurr->next;
        i+= 1;
    }
    printf("Vertex %d: %.2f %.2f\n", i, pcurr->coordinates.x, pcurr->coordinates.y);
}
