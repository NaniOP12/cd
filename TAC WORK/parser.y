%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tempVarCount = 0;
char* newTemp() {
    char* temp = malloc(10);
    sprintf(temp, "t%d", tempVarCount++);
    return temp;
}

void yyerror(const char* s);
int yylex(void);

typedef struct tac {
    char* result;
    char* arg1;
    char* op;
    char* arg2;
} TAC;

void printTAC(TAC* t) {
    printf("%s = %s %s %s\n", t->result, t->arg1, t->op, t->arg2);
}
%}

%union {
    char* str;
    struct tac* code;
}

%token <str> ID NUMBER
%token ASSIGN PLUS MINUS MUL DIV SEMICOLON
%type <str> expr term factor

%%

stmt:
      ID ASSIGN expr SEMICOLON {
          printf("%s = %s\n", $1, $3);
      }
    ;

expr:
      expr PLUS term {
          char* temp = newTemp();
          printf("%s = %s + %s\n", temp, $1, $3);
          $$ = temp;
      }
    | expr MINUS term {
          char* temp = newTemp();
          printf("%s = %s - %s\n", temp, $1, $3);
          $$ = temp;
      }
    | term {
          $$ = $1;
      }
    ;

term:
      term MUL factor {
          char* temp = newTemp();
          printf("%s = %s * %s\n", temp, $1, $3);
          $$ = temp;
      }
    | term DIV factor {
          char* temp = newTemp();
          printf("%s = %s / %s\n", temp, $1, $3);
          $$ = temp;
      }
    | factor {
          $$ = $1;
      }
    ;

factor:
      ID     { $$ = $1; }
    | NUMBER { $$ = $1; }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    printf("Enter the expression (e.g., a = b + c * d;):\n");
    yyparse();
    return 0;
}
