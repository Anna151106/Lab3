#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

struct Var {
    char *name;
    int value;
    int *array;
    int scope;
};

struct Var vars[1000];
int nvars = 0;
int depth = 0;
int returned = 0;
int return_value = 0;
FILE *out;

void error(int line, char *msg, char *name)
{
    fprintf(stderr, "line %d: %s %s\n", line, msg, name);
    exit(1);
}

struct Var *lookup(char *name)
{
    int i;
    for (i = nvars - 1; i >= 0; i--)
        if ((vars[i].scope == depth || vars[i].scope == 0) && strcmp(vars[i].name, name) == 0)
            return &vars[i];
    return NULL;
}

void add_var(char *name, int value, int size)
{
    if (nvars == 1000)
        error(0, "too many variables", name);
    vars[nvars].name = name;
    vars[nvars].value = value;
    vars[nvars].array = size > 0 ? calloc(size, sizeof(int)) : NULL;
    vars[nvars].scope = depth;
    nvars++;
}

void remove_vars(int base)
{
    while (nvars > base) {
        nvars--;
        free(vars[nvars].array);
    }
}

void check(Node *n)
{
    Node *p;
    int base, i;

    switch (n->type) {
    case NODE_PROGRAM:
        for (p = n->child[0]; p != NULL; p = p->next)
            check(p);
        remove_vars(0);
        return;
    case NODE_VAR:
    case NODE_INDEX:
        if (lookup(n->name) == NULL)
            error(n->line, "undeclared variable", n->name);
        break;
    case NODE_DECL:
        if (n->child[0] != NULL)
            check(n->child[0]);
        add_var(n->name, 0, 0);
        return;
    case NODE_FUNC:
        depth = 1;
        base = nvars;
        for (p = n->child[0]; p != NULL; p = p->next)
            add_var(p->name, 0, 0);
        check(n->child[1]);
        remove_vars(base);
        depth = 0;
        return;
    case NODE_BLOCK:
        base = nvars;
        for (p = n->child[0]; p != NULL; p = p->next)
            check(p);
        remove_vars(base);
        return;
    }
    for (i = 0; i < 4; i++)
        for (p = n->child[i]; p != NULL; p = p->next)
            check(p);
}

int *get_address(Node *n)
{
    struct Var *v = lookup(n->name);
    if (v == NULL)
        error(n->line, "undeclared variable", n->name);
    if (n->type == NODE_INDEX)
        return &v->array[eval(n->child[0])];
    return &v->value;
}

int call_func(Node *n)
{
    Node *func, *p;
    int args[20];
    int count = 0, base;

    for (p = n->child[0]; p != NULL; p = p->next)
        args[count++] = eval(p);
    for (func = root->child[0]; func != NULL; func = func->next)
        if (func->type == NODE_FUNC && strcmp(func->name, n->name) == 0)
            break;
    if (func == NULL)
        error(n->line, "undeclared function", n->name);

    base = nvars;
    depth++;
    count = 0;
    for (p = func->child[0]; p != NULL; p = p->next)
        add_var(p->name, args[count++], 0);
    return_value = 0;
    exec(func->child[1]);
    remove_vars(base);
    depth--;
    returned = 0;
    return return_value;
}

int eval(Node *n)
{
    int a, b;
    char *op = n->name;

    switch (n->type) {
    case NODE_NUM:
        return n->value;
    case NODE_VAR:
    case NODE_INDEX:
        return *get_address(n);
    case NODE_CALL:
        return call_func(n);
    case NODE_UNOP:
        a = eval(n->child[0]);
        if (strcmp(op, "-") == 0)
            return -a;
        return !a;
    case NODE_BINOP:
        if (strcmp(op, "&&") == 0)
            return eval(n->child[0]) && eval(n->child[1]);
        if (strcmp(op, "||") == 0)
            return eval(n->child[0]) || eval(n->child[1]);
        a = eval(n->child[0]);
        b = eval(n->child[1]);
        if (strcmp(op, "+") == 0) return a + b;
        if (strcmp(op, "-") == 0) return a - b;
        if (strcmp(op, "*") == 0) return a * b;
        if (strcmp(op, "/") == 0) return a / b;
        if (strcmp(op, "%") == 0) return a % b;
        if (strcmp(op, "<") == 0) return a < b;
        if (strcmp(op, ">") == 0) return a > b;
        if (strcmp(op, "<=") == 0) return a <= b;
        if (strcmp(op, ">=") == 0) return a >= b;
        if (strcmp(op, "==") == 0) return a == b;
        if (strcmp(op, "!=") == 0) return a != b;
    }
    return 0;
}

void do_printf(Node *n)
{
    char *s;
    Node *arg = n->child[0];

    for (s = n->name; *s != '\0'; s++) {
        if (*s == '\\' && s[1] != '\0') {
            s++;
            if (*s == 'n')
                fputc('\n', out);
            else if (*s == 't')
                fputc('\t', out);
            else
                fputc(*s, out);
        } else if (*s == '%' && s[1] != '\0') {
            s++;
            if (*s == 'd' && arg != NULL) {
                fprintf(out, "%d", eval(arg));
                arg = arg->next;
            } else {
                fputc(*s, out);
            }
        } else {
            fputc(*s, out);
        }
    }
}

void exec(Node *n)
{
    Node *p;
    int base, x;

    if (n == NULL || returned)
        return;
    switch (n->type) {
    case NODE_DECL:
        add_var(n->name, n->child[0] != NULL ? eval(n->child[0]) : 0, n->value);
        break;
    case NODE_ASSIGN:
        x = eval(n->child[1]);
        *get_address(n->child[0]) = x;
        break;
    case NODE_INC:
        if (strcmp(n->name, "++") == 0)
            (*get_address(n->child[0]))++;
        else
            (*get_address(n->child[0]))--;
        break;
    case NODE_IF:
        if (eval(n->child[0]))
            exec(n->child[1]);
        else
            exec(n->child[2]);
        break;
    case NODE_WHILE:
        while (!returned && eval(n->child[0]))
            exec(n->child[1]);
        break;
    case NODE_FOR:
        for (exec(n->child[0]); !returned && eval(n->child[1]); exec(n->child[2]))
            exec(n->child[3]);
        break;
    case NODE_RETURN:
        return_value = eval(n->child[0]);
        returned = 1;
        break;
    case NODE_BLOCK:
        base = nvars;
        for (p = n->child[0]; p != NULL && !returned; p = p->next)
            exec(p);
        remove_vars(base);
        break;
    case NODE_PRINTF:
        do_printf(n);
        break;
    default:
        eval(n);
    }
}

void run(Node *prog)
{
    Node *p;

    for (p = prog->child[0]; p != NULL; p = p->next)
        if (p->type == NODE_DECL)
            exec(p);
    call_func(new_node(NODE_CALL, "main", 0, NULL, NULL, NULL, NULL));
    remove_vars(0);
}
