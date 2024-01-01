#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transforms.h"
#include "interpreter.h"

static Stack stack = {.top = 0};

float deg_to_rad(float deg){
    return (deg / 180) * M_PI;
}

TriangleMesh* parse_file(){
    // Open input file
    FILE* pfile = fopen(INPUT_FILE, "r");

    if (pfile == NULL){
        printf("No such file\n");
        exit(1);
    }
    // Rewind in case we already read the file before
    stack.top = 0;

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

    StackElement elem = pop_from_stack();
    return elem.mesh;
}

void parse_token(char* token){
    char* prest;
    float parsed_number = strtof(token, &prest);
    if (prest == token){
        parse_instruction(token);
    } else {
        StackElement elem;
        elem.number = parsed_number;
        push_onto_stack(elem);
    }
    printf("Parsed \"%s\", stack has %d elements\n", token, stack.top);
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
    } else if (strcmp(token, "swap") == 0){
        do_swap();
    } else {
        printf("%s: Unknown instruction. Exiting\n", token);
        exit(1);
    }
}

void push_onto_stack(StackElement elem){
    if (stack.top >= STACK_SIZE) {
        printf("Stack is full\n");
        exit(1);
    } else {
        stack.content[stack.top] = elem;
        ++stack.top;
    }
}

StackElement pop_from_stack(){
    if (stack.top <= 0) {
        printf("Stack is empty\n");
        exit(1);
    } else {
        return stack.content[--stack.top];
    }
}

StackElement peek_stack(){
    if (stack.top <= 0) {
        printf("Stack is empty\n");
        exit(1);
    } else {
        return stack.content[stack.top - 1];
    }
}

void do_box(){
    float c = pop_from_stack().number;
    float b = pop_from_stack().number;
    float a = pop_from_stack().number;
    TriangleMesh* pbox = box(a, b, c);
    StackElement elem;
    elem.mesh = pbox;
    push_onto_stack(elem);
}

void do_rotate(){
    float z = pop_from_stack().number;
    float y = pop_from_stack().number;
    float x = pop_from_stack().number;
    Point3D rotation = {deg_to_rad(x), deg_to_rad(y), deg_to_rad(z)};
    TriangleMesh* mesh = peek_stack().mesh;
    rotate_mesh(mesh, rotation);
}

void do_translate(){
    float z = pop_from_stack().number;
    float y = pop_from_stack().number;
    float x = pop_from_stack().number;
    Point3D translation = {x, y, z};
    TriangleMesh* mesh = peek_stack().mesh;
    translate_mesh(mesh, translation);
}

void do_prism(){
    float height = pop_from_stack().number;
    int n_size = (int) pop_from_stack().number;
    float radius = pop_from_stack().number;
    Polygon* ppoly = new_regular_polygon(radius, n_size);
    TriangleMesh* mesh = prism(ppoly, height);
    StackElement elem;
    elem.mesh = mesh;
    push_onto_stack(elem);
}

void do_merge(){
    TriangleMesh* mesh1 = pop_from_stack().mesh;
    TriangleMesh* mesh2 = pop_from_stack().mesh;
    mesh1 = merge_tri_meshes(mesh1, mesh2);
    StackElement elem;
    elem.mesh = mesh1;
    push_onto_stack(elem);
}

void do_clone(){
    TriangleMesh* mesh = peek_stack().mesh;
    mesh = copy_mesh(mesh);
    StackElement elem;
    elem.mesh = mesh;
    push_onto_stack(elem);
}

void do_swap(){
    StackElement elem1 = pop_from_stack();
    StackElement elem2 = pop_from_stack();
    push_onto_stack(elem1);
    push_onto_stack(elem2);
}
