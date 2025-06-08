%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

[0-9]+      { yylval.num = atoi(yytext); return NUMBER; }  
[\t ]+      ;  /* Ignore spaces and tabs */
\n          return 0;  /* End input */
.           return yytext[0];  /* Return single character tokens */

%%

int yywrap() {
    return 1;
}

