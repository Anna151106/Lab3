#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern FILE *yyin;
int yyparse(void);

char *read_file(FILE *f)
{
    long size;
    char *s;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);
    s = malloc(size + 1);
    size = fread(s, 1, size, f);
    s[size] = '\0';
    return s;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s file.c [--py out.py | --json]\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror(argv[1]);
        return 1;
    }
    yyparse();
    check(root);

    if (argc == 4 && strcmp(argv[2], "--py") == 0) {
        py_out = fopen(argv[3], "w");
        if (py_out == NULL) {
            perror(argv[3]);
            return 1;
        }
        gen_py(root, 0);
        fclose(py_out);
    } else if (argc == 3 && strcmp(argv[2], "--json") == 0) {
        printf("{\"file\": ");
        print_json_string(argv[1]);
        printf(", \"source\": ");
        print_json_string(read_file(yyin));
        printf(", \"ast\": ");
        print_json(root);
        py_out = tmpfile();
        gen_py(root, 0);
        printf(", \"python\": ");
        print_json_string(read_file(py_out));
        out = tmpfile();
        run(root);
        printf(", \"output\": ");
        print_json_string(read_file(out));
        printf("}\n");
    } else {
        out = stdout;
        run(root);
    }
    return 0;
}
