%{
#include <stdio.h>
#include <stdlib.h>

int yylex();        
void yyerror(const char *s); 

%}

%token IDENTIFIER

%%

stmt: IDENTIFIER { printf("Valid variable name\n"); }
    ;

%%

int main() {
    printf("Enter a variable name: ");
    if (yyparse() == 0) {  // If parsing succeeds
        return 0;
    }
    return 1;
}

void yyerror(const char *s) {
    printf("Invalid variable name\n");
}

