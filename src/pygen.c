#include <stdio.h>
#include <string.h>
#include "ast.h"

FILE *py_out;
char *globals[100];
int nglobals = 0;

void print_indent(int indent)
{
    int i;
    for (i = 0; i < indent; i++)
        fprintf(py_out, "    ");
}

int has_var(Node *n, char *name, int is_decl)
{
    Node *p;
    int i;

    if (is_decl && (n->type == NODE_DECL || n->type == NODE_PARAM) && strcmp(n->name, name) == 0)
        return 1;
    if (!is_decl && (n->type == NODE_ASSIGN || n->type == NODE_INC)
        && n->child[0]->type == NODE_VAR && strcmp(n->child[0]->name, name) == 0)
        return 1;
    for (i = 0; i < 4; i++)
        for (p = n->child[i]; p != NULL; p = p->next)
            if (has_var(p, name, is_decl))
                return 1;
    return 0;
}

void gen_expr(Node *n)
{
    Node *p;
    char *op = n->name;

    switch (n->type) {
    case NODE_NUM:
        fprintf(py_out, "%d", n->value);
        break;
    case NODE_VAR:
        fprintf(py_out, "%s", n->name);
        break;
    case NODE_INDEX:
        fprintf(py_out, "%s[", n->name);
        gen_expr(n->child[0]);
        fprintf(py_out, "]");
        break;
    case NODE_CALL:
        fprintf(py_out, "%s(", n->name);
        for (p = n->child[0]; p != NULL; p = p->next) {
            gen_expr(p);
            if (p->next != NULL)
                fprintf(py_out, ", ");
        }
        fprintf(py_out, ")");
        break;
    case NODE_UNOP:
        if (strcmp(op, "-") == 0)
            fprintf(py_out, "(-");
        else
            fprintf(py_out, "int(not ");
        gen_expr(n->child[0]);
        fprintf(py_out, ")");
        break;
    case NODE_BINOP:
        if (strcmp(op, "/") == 0) {
            fprintf(py_out, "int(");
            gen_expr(n->child[0]);
            fprintf(py_out, " / ");
            gen_expr(n->child[1]);
            fprintf(py_out, ")");
        } else if (strcmp(op, "%") == 0) {
            fprintf(py_out, "int(math.fmod(");
            gen_expr(n->child[0]);
            fprintf(py_out, ", ");
            gen_expr(n->child[1]);
            fprintf(py_out, "))");
        } else if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
            fprintf(py_out, "int(bool(");
            gen_expr(n->child[0]);
            fprintf(py_out, ") %s bool(", strcmp(op, "&&") == 0 ? "and" : "or");
            gen_expr(n->child[1]);
            fprintf(py_out, "))");
        } else {
            fprintf(py_out, "(");
            gen_expr(n->child[0]);
            fprintf(py_out, " %s ", op);
            gen_expr(n->child[1]);
            fprintf(py_out, ")");
        }
        break;
    }
}

void gen_py(Node *n, int indent)
{
    Node *p;
    int i, count;

    switch (n->type) {
    case NODE_PROGRAM:
        fprintf(py_out, "import math\n\n");
        for (p = n->child[0]; p != NULL; p = p->next)
            if (p->type == NODE_DECL) {
                gen_py(p, 0);
                globals[nglobals++] = p->name;
            }
        for (p = n->child[0]; p != NULL; p = p->next)
            if (p->type == NODE_FUNC)
                gen_py(p, 0);
        fprintf(py_out, "\nmain()\n");
        break;
    case NODE_FUNC:
        fprintf(py_out, "\ndef %s(", n->name);
        for (p = n->child[0]; p != NULL; p = p->next)
            fprintf(py_out, "%s%s", p->name, p->next != NULL ? ", " : "");
        fprintf(py_out, "):\n");
        count = 0;
        for (i = 0; i < nglobals; i++)
            if (has_var(n, globals[i], 0) && !has_var(n, globals[i], 1)) {
                if (count == 0) {
                    print_indent(indent + 1);
                    fprintf(py_out, "global %s", globals[i]);
                } else {
                    fprintf(py_out, ", %s", globals[i]);
                }
                count++;
            }
        if (count > 0)
            fprintf(py_out, "\n");
        gen_py(n->child[1], indent + 1);
        break;
    case NODE_BLOCK:
        if (n->child[0] == NULL) {
            print_indent(indent);
            fprintf(py_out, "pass\n");
        }
        for (p = n->child[0]; p != NULL; p = p->next)
            gen_py(p, indent);
        break;
    case NODE_DECL:
        print_indent(indent);
        if (n->value > 0) {
            fprintf(py_out, "%s = [0] * %d\n", n->name, n->value);
        } else if (n->child[0] != NULL) {
            fprintf(py_out, "%s = ", n->name);
            gen_expr(n->child[0]);
            fprintf(py_out, "\n");
        } else {
            fprintf(py_out, "%s = 0\n", n->name);
        }
        break;
    case NODE_ASSIGN:
        print_indent(indent);
        gen_expr(n->child[0]);
        fprintf(py_out, " = ");
        gen_expr(n->child[1]);
        fprintf(py_out, "\n");
        break;
    case NODE_INC:
        print_indent(indent);
        gen_expr(n->child[0]);
        fprintf(py_out, strcmp(n->name, "++") == 0 ? " += 1\n" : " -= 1\n");
        break;
    case NODE_IF:
        print_indent(indent);
        fprintf(py_out, "if ");
        gen_expr(n->child[0]);
        fprintf(py_out, ":\n");
        gen_py(n->child[1], indent + 1);
        if (n->child[2] != NULL) {
            print_indent(indent);
            fprintf(py_out, "else:\n");
            gen_py(n->child[2], indent + 1);
        }
        break;
    case NODE_WHILE:
        print_indent(indent);
        fprintf(py_out, "while ");
        gen_expr(n->child[0]);
        fprintf(py_out, ":\n");
        gen_py(n->child[1], indent + 1);
        break;
    case NODE_FOR:
        gen_py(n->child[0], indent);
        print_indent(indent);
        fprintf(py_out, "while ");
        gen_expr(n->child[1]);
        fprintf(py_out, ":\n");
        gen_py(n->child[3], indent + 1);
        gen_py(n->child[2], indent + 1);
        break;
    case NODE_RETURN:
        print_indent(indent);
        fprintf(py_out, "return ");
        gen_expr(n->child[0]);
        fprintf(py_out, "\n");
        break;
    case NODE_PRINTF:
        print_indent(indent);
        fprintf(py_out, "print(\"%s\" %% (", n->name);
        for (p = n->child[0]; p != NULL; p = p->next) {
            gen_expr(p);
            fprintf(py_out, ", ");
        }
        fprintf(py_out, "), end=\"\")\n");
        break;
    default:
        print_indent(indent);
        gen_expr(n);
        fprintf(py_out, "\n");
    }
}
