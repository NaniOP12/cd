%{
#include "y.tab.h"
%}

%%
[a-zA-Z][a-zA-Z0-9]*  { yylval.str = strdup(yytext); return ID; }
[0-9]+                { yylval.str = strdup(yytext); return NUMBER; }
"="                   { return ASSIGN; }
"+"                   { return PLUS; }
"-"                   { return MINUS; }
"*"                   { return MUL; }
"/"                   { return DIV; }
";"                   { return SEMICOLON; }
[ \t\n]               ;  // ignore whitespace
.                     { return yytext[0]; }
%%

int yywrap() { return 1; }
