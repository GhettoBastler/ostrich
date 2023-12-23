#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "primitives.h"
#include "transforms.h"
#include "vect.h"

TriangleMesh* tri_cube(float size){
    TriangleMesh* pres = (TriangleMesh*) malloc(sizeof(TriangleMesh) + 12 * sizeof(Triangle));
    pres->size = 12;

    Point3D a = {0, 0, 0};
    Point3D b = {size, 0, 0};
    Point3D c = {size, size, 0};
    Point3D d = {0, size, 0};
    Point3D e = {0, 0, size};
    Point3D f = {size, 0, size};
    Point3D g = {size, size, size};
    Point3D h = {0, size, size};

    Triangle abc = {a, b, c};
    Triangle cda = {c, d, a};

    Triangle bfg = {b, f, g};
    Triangle gcb = {g, c, b};

    Triangle cgh = {c, g, h};
    Triangle hdc = {h, d, c};

    Triangle ead = {e, a, d};
    Triangle dhe = {d, h, e};

    Triangle feh = {f, e, h};
    Triangle hgf = {h, g, f};

    Triangle efb = {e, f, b};
    Triangle bae = {b, a, e};

    pres->triangles[0] = abc;
    pres->triangles[0].visible[0] = true;
    pres->triangles[0].visible[1] = true;
    pres->triangles[0].visible[2] = false;
    pres->triangles[1] = cda;
    pres->triangles[1].visible[0] = true;
    pres->triangles[1].visible[1] = true;
    pres->triangles[1].visible[2] = false;

    pres->triangles[2] = bfg;
    pres->triangles[2].visible[0] = true;
    pres->triangles[2].visible[1] = true;
    pres->triangles[2].visible[2] = false;
    pres->triangles[3] = gcb;
    pres->triangles[3].visible[0] = true;
    pres->triangles[3].visible[1] = true;
    pres->triangles[3].visible[2] = false;

    pres->triangles[4] = cgh;
    pres->triangles[4].visible[0] = true;
    pres->triangles[4].visible[1] = true;
    pres->triangles[4].visible[2] = false;
    pres->triangles[5] = hdc;
    pres->triangles[5].visible[0] = true;
    pres->triangles[5].visible[1] = true;
    pres->triangles[5].visible[2] = false;

    pres->triangles[6] = ead;
    pres->triangles[6].visible[0] = true;
    pres->triangles[6].visible[1] = true;
    pres->triangles[6].visible[2] = false;
    pres->triangles[7] = dhe;
    pres->triangles[7].visible[0] = true;
    pres->triangles[7].visible[1] = true;
    pres->triangles[7].visible[2] = false;

    pres->triangles[8] = feh;
    pres->triangles[8].visible[0] = true;
    pres->triangles[8].visible[1] = true;
    pres->triangles[8].visible[2] = false;
    pres->triangles[9] = hgf;
    pres->triangles[9].visible[0] = true;
    pres->triangles[9].visible[1] = true;
    pres->triangles[9].visible[2] = false;

    pres->triangles[10] = efb;
    pres->triangles[10].visible[0] = true;
    pres->triangles[10].visible[1] = true;
    pres->triangles[10].visible[2] = false;
    pres->triangles[11] = bae;
    pres->triangles[11].visible[0] = true;
    pres->triangles[11].visible[1] = true;
    pres->triangles[11].visible[2] = false;

    return pres;
}

Polygon* new_polygon(Point2D* vertices, int size){
    Polygon* pres = (Polygon*) malloc(sizeof(Polygon));
    pres->size = size;

    PolygonVertex* phead = (PolygonVertex*) malloc(sizeof(PolygonVertex));
    if (pres == NULL){
        fprintf(stderr, "Couldn\'t allocate memory for the polygon\n");
        exit(1);
    };
    phead->prev = phead;
    phead->next = phead;
    phead->coordinates = vertices[0];

    PolygonVertex* ptail = phead;

    for (int i = 1; i < size; i++){
        PolygonVertex* pcurr = (PolygonVertex*) malloc(sizeof(PolygonVertex));
        if (pcurr == NULL){
            fprintf(stderr, "Couldn\'t allocate memory for the new vertex\n");
            exit(1);
        };
        pcurr->coordinates = vertices[i];
        pcurr->prev = ptail;
        pcurr->next = phead;
        ptail->next = pcurr;
        phead->prev = pcurr;

        ptail = pcurr;
    }

    pres->head = phead;
    return pres;
};

void free_polygon(Polygon* ppoly){
    PolygonVertex* ptmp;
    PolygonVertex* pcurr = ppoly->head;
    PolygonVertex* pnext = pcurr->next;
    while (pcurr->next != ppoly->head){
        ptmp = pcurr;
        pcurr = ptmp->next;
        free(ptmp);
    }
    free(pcurr);
    free(ppoly);
}

void triangulate(Polygon* ppoly){
    // This does nothing for now, except telling you what
    // diagonals it should add

    // First, find the top and bottom-most vertices

    float min_y = INFINITY,
          max_y = -INFINITY;
    PolygonVertex* phead = ppoly->head;
    PolygonVertex* pcurr_vertex = phead;
    PolygonVertex* ptop_vertex;
    PolygonVertex* pbottom_vertex;

    while (pcurr_vertex->next != phead){
        if (pcurr_vertex->coordinates.y < min_y){
            min_y = pcurr_vertex->coordinates.y;
            ptop_vertex = pcurr_vertex;
        }
        if (pcurr_vertex->coordinates.y > max_y){
            max_y = pcurr_vertex->coordinates.y;
            pbottom_vertex = pcurr_vertex;
        }
        pcurr_vertex = pcurr_vertex->next;
    }
    // Do it one last time
    if (pcurr_vertex->coordinates.y < min_y){
        min_y = pcurr_vertex->coordinates.y;
        ptop_vertex = pcurr_vertex;
    }
    if (pcurr_vertex->coordinates.y > max_y){
        max_y = pcurr_vertex->coordinates.y;
        pbottom_vertex = pcurr_vertex;
    }
    pcurr_vertex = pcurr_vertex->next;

    printf("Top vertex is at %.2f %.2f\n", ptop_vertex->coordinates.x, ptop_vertex->coordinates.y);
    printf("Bottom vertex is at %.2f %.2f\n", pbottom_vertex->coordinates.x, pbottom_vertex->coordinates.y);

    // Then, create a list of vertices from top to bottom, and keep track of which "chain"
    // it is on (left=1, right=2. Bottom and top are 0).

    struct MergedChain{
        PolygonVertex* vertex;
        int chain;
    };

    struct MergedChain chain[ppoly->size];
    chain[0].vertex = ptop_vertex;
    chain[0].chain = 0;

    PolygonVertex* curr_left = ptop_vertex->next;
    PolygonVertex* curr_right = ptop_vertex->prev;
    bool cont_left = true,
         cont_right = true;

    int i = 1;
    while (cont_left || cont_right){
        if (cont_left && !cont_right){
            printf("Adding left\n");
            chain[i].vertex = curr_left;
            chain[i].chain = 1;
            curr_left = curr_left->next;
        } else if (cont_right && !cont_left){
            printf("Adding right\n");
            chain[i].vertex = curr_right;
            chain[i].chain = 2;
            curr_right = curr_right->prev;
        } else {
            if (curr_left->coordinates.y == curr_right->coordinates.y){
                // Add leftmost
                printf("Adding left\n");
                chain[i].vertex = curr_left;
                chain[i].chain = 1;
                curr_left = curr_left->next;
            } else if (curr_left->coordinates.y < curr_right->coordinates.y){
                // Left vertex above right vertex
                printf("Adding left\n");
                chain[i].vertex = curr_left;
                chain[i].chain = 1;
                curr_left = curr_left->next;
            } else {
                printf("Adding right\n");
                chain[i].vertex = curr_right;
                chain[i].chain = 2;
                curr_right = curr_right->prev;
            }
        }
        i += 1;
        cont_left = curr_left != pbottom_vertex;
        cont_right = curr_right != pbottom_vertex;
    }

    // Last node
    chain[i].vertex = pbottom_vertex;
    chain[i].chain = 0;


    // Printing merged chain
    for (i = 0; i < ppoly->size; i++){
        printf("%d: y is %.2f on chain %d\n", i, chain[i].vertex->coordinates.y, chain[i].chain);
    }

    // Next, create a stack...
    struct VertexStack {
        int top;
        struct MergedChain content[ppoly->size];
    };

    struct VertexStack stack;
    stack.top = -1;

    void push(struct MergedChain vert){
        stack.top += 1;
        stack.content[stack.top] = vert;
    }

    struct MergedChain pop(){
        struct MergedChain res = stack.content[stack.top];
        stack.top -= 1;
        return res;
    }

    struct MergedChain peek(){
        struct MergedChain res = stack.content[stack.top];
        return res;
    }

    bool stack_empty(){
        return stack.top < 0;
    }
    
    //... and push the first two nodes onto it
    push(chain[0]);
    push(chain[1]);

    struct MergedChain curr_vertex, prev_vertex;

    // Next, go through all the remaining nodes from the merged chain
    for (i = 2; i < ppoly->size - 1; i++){
        // Are the current node and the one on top of the stack on the same chain ?
        if (chain[i].chain != peek().chain){
            // Different chains
            // Pop all the vertices from the stack
            while (1){
                curr_vertex = pop();
                if (stack_empty()){
                    // Last item, don't join
                    break;
                } else {
                    printf("Triangle:\n\t%.2f %.2f\n\t%.2f %.2f\n\t%.2f %.2f\n",
                           chain[i].vertex->coordinates.x,
                           chain[i].vertex->coordinates.y,
                           curr_vertex.vertex->coordinates.x,
                           curr_vertex.vertex->coordinates.y,
                           peek().vertex->coordinates.x,
                           peek().vertex->coordinates.y);
                }
            }
            // Push the last two vertices
            push(chain[i-1]);
            push(chain[i]);
        } else {
            // Same chain
            curr_vertex = pop();
            while (!stack_empty()){
                prev_vertex = curr_vertex;
                curr_vertex = pop();
                // Check if the diagonal is inside P
                Point3D v1 = {prev_vertex.vertex->coordinates.x - chain[i].vertex->coordinates.x,
                              prev_vertex.vertex->coordinates.y - chain[i].vertex->coordinates.y,
                              0};
                Point3D v2 = {curr_vertex.vertex->coordinates.x - chain[i].vertex->coordinates.x,
                              curr_vertex.vertex->coordinates.y - chain[i].vertex->coordinates.y,
                              0};
                Point3D cross = cross_product(v1, v2);
                if (curr_vertex.chain == 1 && cross.z < 0){
                    // Left chain, can join
                    printf("Triangle:\n\t%.2f %.2f\n\t%.2f %.2f\n\t%.2f %.2f\n",
                           chain[i].vertex->coordinates.x,
                           chain[i].vertex->coordinates.y,
                           curr_vertex.vertex->coordinates.x,
                           curr_vertex.vertex->coordinates.y,
                           prev_vertex.vertex->coordinates.x,
                           prev_vertex.vertex->coordinates.y);
                } else if (curr_vertex.chain == 2 && cross.z > 0){
                    // Right chain, can join
                    printf("Triangle:\n\t%.2f %.2f\n\t%.2f %.2f\n\t%.2f %.2f\n",
                           chain[i].vertex->coordinates.x,
                           chain[i].vertex->coordinates.y,
                           curr_vertex.vertex->coordinates.x,
                           curr_vertex.vertex->coordinates.y,
                           prev_vertex.vertex->coordinates.x,
                           prev_vertex.vertex->coordinates.y);
                } else {
                    // Can't join
                    // Put it back
                    push(curr_vertex);
                    curr_vertex = prev_vertex;
                    break;
                }
            }
            push(curr_vertex);
            push(chain[i]);
        }
    }
    // Add diagonals from un to all stack vertices except the ﬁrst and the last one
    if (!stack_empty()){
        // pop();
        prev_vertex = pop();
        if (!stack_empty()){
            while(!stack_empty()){
                if (stack_empty())
                // Last one
                    break;
                else {
                    curr_vertex = pop();
                    // printf("Will draw a diagonal from x:%.2f y:%.2f to x:%.2f y:%.2f\n",
                    //         chain[ppoly->size - 1].vertex->coordinates.x,
                    //         chain[ppoly->size - 1].vertex->coordinates.y,
                    //         curr_vertex.vertex->coordinates.x,
                    //         curr_vertex.vertex->coordinates.y);
                    printf("Triangle:\n\t%.2f %.2f\n\t%.2f %.2f\n\t%.2f %.2f\n",
                           chain[ppoly->size - 1].vertex->coordinates.x,
                           chain[ppoly->size - 1].vertex->coordinates.y,
                           curr_vertex.vertex->coordinates.x,
                           curr_vertex.vertex->coordinates.y,
                           prev_vertex.vertex->coordinates.x,
                           prev_vertex.vertex->coordinates.y);
                    prev_vertex = curr_vertex;
                }

            }
        }
    }
}
