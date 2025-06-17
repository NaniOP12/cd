/* lexer.l */

%{
#include "y.tab.h" // Include Yacc's generated header for token definitions
#include <stdio.h>
%}

%%

[0-9]+      { yylval = atoi(yytext); return NUM; } // Matches numbers, stores their integer value in yylval, returns NUM token
"+"         { return PLUS; }                       // Matches '+', returns PLUS token
[ \t\n]+    { /* Ignore whitespace */ }
.           { printf("Lexical error: Unexpected character '%s'\n", yytext); return yytext[0]; } // Catch any other character

%%

// Required for yylex function (often linked from flex library)
int yywrap() {
    return 1;
}
