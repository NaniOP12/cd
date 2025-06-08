%{
#include <stdio.h>
#include <stdlib.h>

int yylex();        
void yyerror(const char *s); 

%}

%token NUMBER

%%

expr: expr '+' term  
    | expr '-' term  
    | term            
    ;

term: term '*' factor  
    | term '/' factor  
    | factor           
    ;

factor: '(' expr ')'  
      | NUMBER         
      ;

%%

int main() {
    printf("Enter an arithmetic expression: ");
    if (yyparse() == 0) {  // If parsing is successful
        printf("Valid expression\n");
    }
    return 0;
}

void yyerror(const char *s) {
    printf("Invalid expression\n");
}

