%option noyywrap 
%{ 
#include <stdio.h> 
%} 
 
%x PRINTF 
 
DIGIT       [0-9] 
LETTER      [a-zA-Z] 
ID          {LETTER}({LETTER}|{DIGIT})* 
NUMBER      {DIGIT}+(\.{DIGIT}+)? 
WHITESPACE  [ \t\n] 
STRING      \"([^\\\"]|\\.)*\" 
 
%% 
 
^#.*                               { printf("PREPROCESSOR: %s\n", yytext); } 
 
"printf"[ \t]*\(                  { printf("LIBRARY FUNCTION: printf\nSYMBOL: (\n")); 
BEGIN(PRINTF); } 
 
"int"|"float"|"return"|"if"|"else"|"while"|"for"|"void" { printf("KEYWORD: %s\n", yytext); 
} 
 
"printf"                           { printf("LIBRARY FUNCTION: %s\n", yytext); } 
 
{ID}                               { printf("IDENTIFIER: %s\n", yytext); } 
 
{NUMBER}                           { printf("NUMBER: %s\n", yytext); } 
 
"=="|"!="|"<="|">="|"="|"+"|"-"|"*"|"/"|"<"|">" { printf("OPERATOR: %s\n", yytext); } 
 
"("|")"|"{"|"}"|";"|","            { printf("SYMBOL: %s\n", yytext); } 
 
{STRING}                           { printf("STRING: %s\n", yytext); } 
 
<PRINTF>{ 
    {STRING}                       { printf("STRING: %s\n", yytext); } 
    ","                            { printf("SYMBOL: ,\n"); } 
    {ID}                           { printf("IDENTIFIER: %s\n", yytext); } 
    {NUMBER}                       { printf("NUMBER: %s\n", yytext); } 
    ")"                            { printf("SYMBOL: )\n"); BEGIN(INITIAL); } 
    [ \t\n]+                       { } 
    .                              { printf("UNKNOWN: %s\n", yytext); } 
} 
 
{WHITESPACE}                       { } 
 
.                                  { printf("UNKNOWN: %s\n", yytext); } 
%% 
int main(int argc, char **argv) { 
    if (argc > 1) { 
        FILE *file = fopen(argv[1], "r"); 
        if (!file) { 
            perror("Error opening file"); 
            return 1; 
        } 
        yyin = file; 
    } 
    yylex(); 
    if (argc > 1) { 
        fclose(yyin); 
    } 
    return 0; 
}