/* parser.y */

%{
#include <stdio.h>
#include <stdlib.h>

// Global variable for lexical analysis
extern int yylex();
extern int yyparse();
extern FILE *yyin;

// Error handling function
void yyerror(const char *s) {
    fprintf(stderr, "Parsing error: %s\n", s);
}
%}

// Token declarations from lexer.l
%token NUM PLUS

// Define the start symbol of the grammar
%start expr

%% // Grammar rules section

// Grammar:
// E  -> E + T
// E  -> T
// T  -> num

expr:   expr PLUS term    { printf("Reduction: E -> E + T\n"); }
      | term              { printf("Reduction: E -> T\n"); }
      ;

term:   NUM               { printf("Reduction: T -> num (value: %d)\n", $1); }
      ;

%% // C code section

// main function to open input file and start parsing
int main() {
    printf("Enter an arithmetic expression (e.g., 5 + 10): \n");
    yyin = stdin; // Set input to standard input

    // Start parsing
    if (yyparse() == 0) {
        printf("Parsing successful!\n");
    } else {
        printf("Parsing failed.\n");
    }

    return 0;
}
