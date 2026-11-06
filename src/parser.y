%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int num;
    char *str;
    struct Node *node;
}

%token <num> NUM
%token <str> ID STRING
%token INT IF ELSE WHILE FOR RETURN PRINTF
%token INC DEC LE GE EQ NE AND OR

%type <node> items item decl func params param block stmts stmt simple_stmt var expr args arg_list print_args

%nonassoc NO_ELSE
%nonassoc ELSE
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right '!' NEG

%%

program
    : items                         { root = new_node(NODE_PROGRAM, NULL, 0, $1, NULL, NULL, NULL); }
    ;

items
    : item                          { $$ = $1; }
    | items item                    { $$ = append($1, $2); }
    ;

item
    : decl                          { $$ = $1; }
    | func                          { $$ = $1; }
    ;

decl
    : INT ID ';'                    { $$ = new_node(NODE_DECL, $2, 0, NULL, NULL, NULL, NULL); }
    | INT ID '=' expr ';'           { $$ = new_node(NODE_DECL, $2, 0, $4, NULL, NULL, NULL); }
    | INT ID '[' NUM ']' ';'        { $$ = new_node(NODE_DECL, $2, $4, NULL, NULL, NULL, NULL); }
    ;

func
    : INT ID '(' params ')' block   { $$ = new_node(NODE_FUNC, $2, 0, $4, $6, NULL, NULL); }
    | INT ID '(' ')' block          { $$ = new_node(NODE_FUNC, $2, 0, NULL, $5, NULL, NULL); }
    ;

params
    : param                         { $$ = $1; }
    | params ',' param              { $$ = append($1, $3); }
    ;

param
    : INT ID                        { $$ = new_node(NODE_PARAM, $2, 0, NULL, NULL, NULL, NULL); }
    ;

block
    : '{' stmts '}'                 { $$ = new_node(NODE_BLOCK, NULL, 0, $2, NULL, NULL, NULL); }
    ;

stmts
    :                               { $$ = NULL; }
    | stmts stmt                    { $$ = append($1, $2); }
    ;

stmt
    : decl                          { $$ = $1; }
    | simple_stmt ';'               { $$ = $1; }
    | PRINTF '(' STRING print_args ')' ';'  { $$ = new_node(NODE_PRINTF, $3, 0, $4, NULL, NULL, NULL); }
    | IF '(' expr ')' stmt %prec NO_ELSE  { $$ = new_node(NODE_IF, NULL, 0, $3, $5, NULL, NULL); }
    | IF '(' expr ')' stmt ELSE stmt  { $$ = new_node(NODE_IF, NULL, 0, $3, $5, $7, NULL); }
    | WHILE '(' expr ')' stmt       { $$ = new_node(NODE_WHILE, NULL, 0, $3, $5, NULL, NULL); }
    | FOR '(' simple_stmt ';' expr ';' simple_stmt ')' stmt  { $$ = new_node(NODE_FOR, NULL, 0, $3, $5, $7, $9); }
    | RETURN expr ';'               { $$ = new_node(NODE_RETURN, NULL, 0, $2, NULL, NULL, NULL); }
    | block                         { $$ = $1; }
    ;

simple_stmt
    : var '=' expr                  { $$ = new_node(NODE_ASSIGN, NULL, 0, $1, $3, NULL, NULL); }
    | var INC                       { $$ = new_node(NODE_INC, "++", 0, $1, NULL, NULL, NULL); }
    | var DEC                       { $$ = new_node(NODE_INC, "--", 0, $1, NULL, NULL, NULL); }
    | ID '(' args ')'               { $$ = new_node(NODE_CALL, $1, 0, $3, NULL, NULL, NULL); }
    ;

var
    : ID                            { $$ = new_node(NODE_VAR, $1, 0, NULL, NULL, NULL, NULL); }
    | ID '[' expr ']'               { $$ = new_node(NODE_INDEX, $1, 0, $3, NULL, NULL, NULL); }
    ;

print_args
    :                               { $$ = NULL; }
    | print_args ',' expr           { $$ = append($1, $3); }
    ;

args
    :                               { $$ = NULL; }
    | arg_list                      { $$ = $1; }
    ;

arg_list
    : expr                          { $$ = $1; }
    | arg_list ',' expr             { $$ = append($1, $3); }
    ;

expr
    : NUM                           { $$ = new_node(NODE_NUM, NULL, $1, NULL, NULL, NULL, NULL); }
    | var                           { $$ = $1; }
    | ID '(' args ')'               { $$ = new_node(NODE_CALL, $1, 0, $3, NULL, NULL, NULL); }
    | '(' expr ')'                  { $$ = $2; }
    | '-' expr %prec NEG            { $$ = new_node(NODE_UNOP, "-", 0, $2, NULL, NULL, NULL); }
    | '!' expr                      { $$ = new_node(NODE_UNOP, "!", 0, $2, NULL, NULL, NULL); }
    | expr '+' expr                 { $$ = new_node(NODE_BINOP, "+", 0, $1, $3, NULL, NULL); }
    | expr '-' expr                 { $$ = new_node(NODE_BINOP, "-", 0, $1, $3, NULL, NULL); }
    | expr '*' expr                 { $$ = new_node(NODE_BINOP, "*", 0, $1, $3, NULL, NULL); }
    | expr '/' expr                 { $$ = new_node(NODE_BINOP, "/", 0, $1, $3, NULL, NULL); }
    | expr '%' expr                 { $$ = new_node(NODE_BINOP, "%", 0, $1, $3, NULL, NULL); }
    | expr '<' expr                 { $$ = new_node(NODE_BINOP, "<", 0, $1, $3, NULL, NULL); }
    | expr '>' expr                 { $$ = new_node(NODE_BINOP, ">", 0, $1, $3, NULL, NULL); }
    | expr LE expr                  { $$ = new_node(NODE_BINOP, "<=", 0, $1, $3, NULL, NULL); }
    | expr GE expr                  { $$ = new_node(NODE_BINOP, ">=", 0, $1, $3, NULL, NULL); }
    | expr EQ expr                  { $$ = new_node(NODE_BINOP, "==", 0, $1, $3, NULL, NULL); }
    | expr NE expr                  { $$ = new_node(NODE_BINOP, "!=", 0, $1, $3, NULL, NULL); }
    | expr AND expr                 { $$ = new_node(NODE_BINOP, "&&", 0, $1, $3, NULL, NULL); }
    | expr OR expr                  { $$ = new_node(NODE_BINOP, "||", 0, $1, $3, NULL, NULL); }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "line %d: %s\n", yylineno, s);
    exit(1);
}
