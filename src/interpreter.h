#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdio.h>
#include "primitives.h"

#define STACK_SIZE 512
#define INPUT_FILE "my_code"
#define BUFFER_SIZE 512

typedef struct {
    int top;
    TriangleMesh* content[STACK_SIZE];
} ObjectStack;

typedef struct {
    int top;
    float content[STACK_SIZE];
} WorkStack;

static const char* delimiter = " \n";

TriangleMesh* mesh_from_file(FILE* pfile);
void push_onto_work_stack(float elem);
void push_onto_obj_stack(TriangleMesh* elem);
float pop_from_work_stack();
TriangleMesh* pop_from_obj_stack();
void parse_token(char* token);
void parse_instruction(char* token);

// Instructions
void do_box();
void do_prism();
void do_rotate();
void do_translate();
void do_merge();
void do_clone();
void do_swap_obj();
void do_swap_work();
void do_rot_work();
void do_rot_obj();
void do_rand();
void do_add();
void do_sub();
void do_mul();
void do_div();

#endif
