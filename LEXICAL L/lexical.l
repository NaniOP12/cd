%{
#include <stdio.h>
%}

%%


int|float|return|if|else { printf("Keyword: %s\n", yytext); }


[a-zA-Z_][a-zA-Z0-9_]* { printf("Identifier: %s\n", yytext); }


[0-9]+(\.[0-9]+)? { printf("Constant: %s\n", yytext); }


==|=|\+|\-|\*|\/ { printf("Operator: %s\n", yytext); }



,|; { printf("Delimiter: %s\n", yytext); }

\(|\)|\{|\} { printf("Special Symbol: %s\n", yytext); }


[ \t\n]+ { }


. { printf("Unknown character: %s\n", yytext); }

%%

int main() {
    yylex();  
    return 0;
}

int yywrap() {
    return 1;  
}


/*

input.txt

int main() {
    float x = 10.5, y = 20;
    int sum = x + y;
    if (sum > 30) {
        return sum;
    }
}

*/