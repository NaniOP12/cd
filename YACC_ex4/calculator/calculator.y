%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);

int result; // Store final computed result
%}

%union {
    int num;
}

/* Define token types */
%token <num> NUMBER

/* Define expression type */
%type <num> expr

/* Operator precedence and associativity */
%left '+' '-'
%left '*' '/'
%right UMINUS /* Unary minus */

%%

input:
    expr { result = $1; }  // Store computed result
    ;

expr: expr '+' expr  { $$ = $1 + $3; }
    | expr '-' expr  { $$ = $1 - $3; }
    | expr '*' expr  { $$ = $1 * $3; }
    | expr '/' expr  { 
        if ($3 == 0) {
            yyerror("Division by zero");
            YYABORT;
        }
        $$ = $1 / $3;
      }
    | '(' expr ')'    { $$ = $2; }
    | '-' expr %prec UMINUS  { $$ = -$2; }  /* Correct unary minus */
    | NUMBER          { $$ = $1; }
    ;

%%

int main() {
    printf("Enter an arithmetic expression: ");
    if (yyparse() == 0) {  
        printf("Result: %d\n", result);  // Print the computed result
    }
    return 0;
}

void yyerror(const char *s) {
    printf("Error: %s\n", s);
}

