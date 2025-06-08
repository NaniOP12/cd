%{
#include "y.tab.h"
%}

%%

[a-zA-Z][a-zA-Z0-9]*   { return IDENTIFIER; }  /* Match a valid variable */
\n                     { return 0; }  /* Stop parsing after one input */
[ \t]+                 { /* Ignore spaces */ }
.                      { return yytext[0]; }  /* Pass invalid characters */

%%

int yywrap() { return 1; }

