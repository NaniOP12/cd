%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%%

[0-9]+         { yylval.ival = atoi(yytext); return NUMBER; }
[ \t]          { /* Skip whitespace */ }
\n             { return '\n'; }
"+"            { return '+'; }
"-"            { return '-'; }
"*"            { return '*'; }
"/"            { return '/'; }
"("            { return '('; }
")"            { return ')'; }

.              { return yytext[0]; }  

%%

int yywrap(void) {
    return 1;
}

