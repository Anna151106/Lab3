#include <stdio.h>
#include "ast.h"

char *type_names[] = {
    "PROGRAM", "NUM", "VAR", "INDEX", "BINOP", "UNOP", "CALL", "ASSIGN", "INC",
    "IF", "WHILE", "FOR", "RETURN", "FUNC", "PARAM", "DECL", "BLOCK", "PRINTF"
};

void print_json_chars(char *s)
{
    for (; *s != '\0'; s++) {
        if (*s == '"')
            printf("\\\"");
        else if (*s == '\\')
            printf("\\\\");
        else if (*s == '\n')
            printf("\\n");
        else if (*s == '\t')
            printf("\\t");
        else if ((unsigned char)*s < 32)
            printf("\\u%04x", *s);
        else
            putchar(*s);
    }
}

void print_json_string(char *s)
{
    putchar('"');
    print_json_chars(s);
    putchar('"');
}

void print_json(Node *n)
{
    Node *p;
    int i, first = 1;

    printf("{\"name\": \"%s", type_names[n->type]);
    if (n->name != NULL) {
        putchar(' ');
        print_json_chars(n->name);
    }
    if (n->type == NODE_NUM)
        printf(" %d", n->value);
    if (n->type == NODE_DECL && n->value > 0)
        printf("[%d]", n->value);
    printf("\", \"children\": [");
    for (i = 0; i < 4; i++)
        for (p = n->child[i]; p != NULL; p = p->next) {
            if (!first)
                printf(", ");
            print_json(p);
            first = 0;
        }
    printf("]}");
}
