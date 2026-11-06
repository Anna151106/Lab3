#ifndef AST_H
#define AST_H

#include <stdio.h>

enum {
    NODE_PROGRAM,
    NODE_NUM,
    NODE_VAR,
    NODE_INDEX,
    NODE_BINOP,
    NODE_UNOP,
    NODE_CALL,
    NODE_ASSIGN,
    NODE_INC,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_RETURN,
    NODE_FUNC,
    NODE_PARAM,
    NODE_DECL,
    NODE_BLOCK,
    NODE_PRINTF
};

struct Node {
    int type;
    char *name;
    int value;
    struct Node *child[4];
    struct Node *next;
    int line;
};

typedef struct Node Node;

extern Node *root;
extern int yylineno;
extern FILE *out;
extern FILE *py_out;

Node *new_node(int type, char *name, int value, Node *a, Node *b, Node *c, Node *d);
Node *append(Node *list, Node *n);

void check(Node *n);
void run(Node *prog);
int eval(Node *n);
void exec(Node *n);

void gen_py(Node *n, int indent);

void print_json_string(char *s);
void print_json(Node *n);

#endif
