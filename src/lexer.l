%{
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.h"
%}

%option yylineno noyywrap nounput noinput

%%

^#.*                        ;
"//".*                      ;
"/*"([^*]|\*+[^*/])*\*+"/"  ;
[ \t\r\n]+                  ;

"int"       return INT;
"if"        return IF;
"else"      return ELSE;
"while"     return WHILE;
"for"       return FOR;
"return"    return RETURN;
"printf"    return PRINTF;

"++"        return INC;
"--"        return DEC;
"<="        return LE;
">="        return GE;
"=="        return EQ;
"!="        return NE;
"&&"        return AND;
"||"        return OR;

[0-9]+                  { yylval.num = atoi(yytext); return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str = strdup(yytext); return ID; }
\"(\\.|[^"\\\n])*\"     { yylval.str = strndup(yytext + 1, yyleng - 2); return STRING; }

[-+*/%<>=!(){}\[\];,]   return yytext[0];

.   { fprintf(stderr, "line %d: unknown symbol %s\n", yylineno, yytext); exit(1); }

%%
