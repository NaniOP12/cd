%{
#include "y.tab.h"
%}

%%

[0-9]+      { yylval = atoi(yytext); return NUMBER; }  /* Match numbers */
[+\-*/()]   { return yytext[0]; }  /* Match operators and parentheses */
\n          { return 0; }  /* Stop parsing after one line */
[ \t]+      { /* Ignore spaces and tabs */ }
.           { return yytext[0]; }  /* Pass single characters */

%%

int yywrap() { return 1; }

