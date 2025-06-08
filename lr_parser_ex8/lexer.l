%{
#include "y.tab.h"
#include <stdlib.h>
%}

%%

[0-9]+ { yylval.ival = atoi(yytext); return NUMBER; }
[ \t\n] { /* skip whitespace */ }
. { return yytext[0]; }

%%

int yywrap() {
    return 1;
}