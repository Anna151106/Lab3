#include <stdlib.h>
#include "ast.h"

Node *root;

Node *new_node(int type, char *name, int value, Node *a, Node *b, Node *c, Node *d)
{
    Node *n = calloc(1, sizeof(Node));
    n->type = type;
    n->name = name;
    n->value = value;
    n->child[0] = a;
    n->child[1] = b;
    n->child[2] = c;
    n->child[3] = d;
    n->next = NULL;
    n->line = yylineno;
    return n;
}

Node *append(Node *list, Node *n)
{
    Node *p;
    if (list == NULL)
        return n;
    for (p = list; p->next != NULL; p = p->next)
        ;
    p->next = n;
    return list;
}
