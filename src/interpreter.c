#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "transforms.h"
#include "interpreter.h"

static WorkStack wstack = {.top = 0};
static ObjectStack ostack = {.top = 0};
static TriangleMesh* pmesh;

float deg_to_rad(float deg){
    return (deg / 180) * M_PI;
}

TriangleMesh* parse_file(){
    pmesh = new_triangle_mesh(0);

    // Seed random
    srand(time(NULL));
    // Open input file
    FILE* pfile = fopen(INPUT_FILE, "r");

    if (pfile == NULL){
        printf("No such file\n");
        exit(1);
    }
    // Rewind in case we already read the file before
    wstack.top = 0;
    ostack.top = 0;

    // Read the file and parse the tokens
    char buffer[BUFFER_SIZE];
    char* token = NULL;
    char* saveptr = buffer;
    char* read;

    read = fgets(buffer, BUFFER_SIZE, pfile);
    printf("Reading file\n");

    while (read != NULL){
        token = strtok_r(buffer, delimiter, &saveptr);

        while (token != NULL){
            parse_token(token);
            token = strtok_r(NULL, delimiter, &saveptr);
        }
        read = fgets(buffer, BUFFER_SIZE, pfile);
    }

    if (feof(pfile)){
        printf("---\n");
        printf("Reached end of file, closing.\n");
        fclose(pfile);
    }

    //TriangleMesh* mesh = pop_from_obj_stack();
    //return mesh;
    return pmesh;
}

void parse_token(char* token){
    char* prest;
    float parsed_number = strtof(token, &prest);
    if (prest == token){
        parse_instruction(token);
    } else {
        push_onto_work_stack(parsed_number);
    }
    printf("Parsed \"%s\", work stack has %d elements, obj stack has %d\n", token, wstack.top, ostack.top);
}

void parse_instruction(char* token){
    if (strcmp(token, "box") == 0){
        do_box();
    } else if (strcmp(token, "prism") == 0){
        do_prism();
    } else if (strcmp(token, "merge") == 0){
        do_merge();
    } else if (strcmp(token, "rotate") == 0){
        do_rotate();
    } else if (strcmp(token, "translate") == 0){
        do_translate();
    } else if (strcmp(token, "clone") == 0){
        do_clone();
    } else if (strcmp(token, "swap_obj") == 0){
        do_swap_obj();
    } else if (strcmp(token, "swap_work") == 0){
        do_swap_work();
    } else if (strcmp(token, "rot_work") == 0){
        do_rot_work();
    } else if (strcmp(token, "rot_obj") == 0){
        do_rot_obj();
    } else if (strcmp(token, "rand") == 0){
        do_rand();
    } else if (strcmp(token, "render") == 0){
        do_render();
    } else {
        printf("%s: Unknown instruction. Exiting\n", token);
        exit(1);
    }
}

void push_onto_work_stack(float elem){
    if (wstack.top >= STACK_SIZE) {
        printf("Work stack is full\n");
        exit(1);
    } else {
        wstack.content[wstack.top++] = elem;
    }
}

void push_onto_obj_stack(TriangleMesh* elem){
    if (ostack.top >= STACK_SIZE) {
        printf("Object stack is full\n");
        exit(1);
    } else {
        ostack.content[ostack.top++] = elem;
    }
}

float pop_from_work_stack(){
    if (wstack.top <= 0) {
        printf("Work stack is empty\n");
        exit(1);
    } else {
        return wstack.content[--wstack.top];
    }
}

TriangleMesh* pop_from_obj_stack(){
    if (ostack.top <= 0) {
        printf("Object stack is empty\n");
        exit(1);
    } else {
        return ostack.content[--ostack.top];
    }
}

void do_box(){
    float c = pop_from_work_stack();
    float b = pop_from_work_stack();
    float a = pop_from_work_stack();
    TriangleMesh* pbox = box(a, b, c);
    push_onto_obj_stack(pbox);
}

void do_rotate(){
    float z = pop_from_work_stack();
    float y = pop_from_work_stack();
    float x = pop_from_work_stack();
    Point3D rotation = {deg_to_rad(x), deg_to_rad(y), deg_to_rad(z)};
    TriangleMesh* mesh = pop_from_obj_stack();
    rotate_mesh(mesh, rotation);
    push_onto_obj_stack(mesh);
}

void do_translate(){
    float z = pop_from_work_stack();
    float y = pop_from_work_stack();
    float x = pop_from_work_stack();
    Point3D translation = {x, y, z};
    TriangleMesh* mesh = pop_from_obj_stack();
    translate_mesh(mesh, translation);
    push_onto_obj_stack(mesh);
}

void do_prism(){
    float height = pop_from_work_stack();
    int n_size = (int) pop_from_work_stack();
    float radius = pop_from_work_stack();
    Polygon* ppoly = new_regular_polygon(radius, n_size);
    TriangleMesh* mesh = prism(ppoly, height);
    push_onto_obj_stack(mesh);
    free(ppoly);
}

void do_merge(){
    TriangleMesh* mesh1 = pop_from_obj_stack();
    TriangleMesh* mesh2 = pop_from_obj_stack();
    mesh1 = merge_tri_meshes(mesh1, mesh2);
    push_onto_obj_stack(mesh1);
}

void do_clone(){
    TriangleMesh* mesh1 = pop_from_obj_stack();
    TriangleMesh* mesh2 = copy_mesh(mesh1);
    push_onto_obj_stack(mesh1);
    push_onto_obj_stack(mesh2);
}

void do_swap_obj(){
    TriangleMesh* mesh1 = pop_from_obj_stack();
    TriangleMesh* mesh2 = pop_from_obj_stack();
    push_onto_obj_stack(mesh1);
    push_onto_obj_stack(mesh2);
}

void do_swap_work(){
    float n1 = pop_from_work_stack();
    float n2 = pop_from_work_stack();
    push_onto_work_stack(n1);
    push_onto_work_stack(n2);
}

void do_rot_work(){
    float n1 = pop_from_work_stack();
    float n2 = pop_from_work_stack();
    float n3 = pop_from_work_stack();
    push_onto_work_stack(n1);
    push_onto_work_stack(n3);
    push_onto_work_stack(n2);
}

void do_rot_obj(){
    TriangleMesh* mesh1 = pop_from_obj_stack();
    TriangleMesh* mesh2 = pop_from_obj_stack();
    TriangleMesh* mesh3 = pop_from_obj_stack();
    push_onto_obj_stack(mesh1);
    push_onto_obj_stack(mesh3);
    push_onto_obj_stack(mesh2);
}

void do_rand(){
    float max = pop_from_work_stack();
    float min = pop_from_work_stack();
    float res = min + ((float) rand() / (float) (RAND_MAX / (max - min)));
    push_onto_work_stack(res);
}

void do_render(){
    TriangleMesh* mesh = pop_from_obj_stack();
    pmesh = merge_tri_meshes(pmesh, mesh);
}
