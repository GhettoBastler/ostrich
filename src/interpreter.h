#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "primitives.h"

#define STACK_SIZE 512
#define INPUT_FILE "my_code"
#define BUFFER_SIZE 512

typedef union {
    TriangleMesh* mesh;
    float number;
} StackElement;

typedef struct {
    int top;
    StackElement content[STACK_SIZE];
} Stack;

static const char* delimiter = " \n";

TriangleMesh* parse_file();
void push_onto_stack(StackElement elem);
StackElement pop_from_stack();
StackElement peek_stack();
void parse_token(char* token);
void parse_instruction(char* token);
// Instructions
void do_box();
void do_prism();
void do_rotate();
void do_translate();
void do_merge();
void do_clone();

#endif
