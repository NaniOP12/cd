%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
int yyerror(char *s);

typedef struct AST {
    char op;              
    int value;            
    struct AST* left;
    struct AST* right;
} AST;

AST* create_node(char op, AST* left, AST* right, int value) {
    AST* node = (AST*)malloc(sizeof(AST));
    node->op = op;
    node->left = left;
    node->right = right;
    node->value = value;
    return node;
}

void print_ast(AST* node, int level) {
    if (!node) return;
    print_ast(node->right, level + 1);
    for (int i = 0; i < level; ++i) printf("    ");
    if (node->op == 0) printf("%d\n", node->value);
    else printf("%c\n", node->op);
    print_ast(node->left, level + 1);
}

int eval(AST* node) {
    if (!node) return 0;
    if (node->op == 0) return node->value;
    int left = eval(node->left);
    int right = eval(node->right);
    switch (node->op) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/':
            if (right == 0) {
                printf("Error: Division by zero.\n");
                exit(1);
            }
            return left / right;
        default: return 0;
    }
}
%}

%union {
    int ival;
    struct AST* ast;
}

%token <ival> NUMBER
%left '+' '-'
%left '*' '/'
%token '(' ')'
%type <ast> expr

%%

program:
    expr '\n' {
        printf("=== AST ===\n");
        print_ast($1, 0);
        printf("\nFinal Answer: %d\n", eval($1));
    }
;

expr:
    expr '+' expr { $$ = create_node('+', $1, $3, 0); }
    | expr '-' expr { $$ = create_node('-', $1, $3, 0); }
    | expr '*' expr { $$ = create_node('*', $1, $3, 0); }
    | expr '/' expr { $$ = create_node('/', $1, $3, 0); }
    | '(' expr ')'  { $$ = $2; }
    | NUMBER        { $$ = create_node(0, NULL, NULL, $1); }
;

%%

int yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    return 0;
}

int main() {
    printf("Enter an expression (e.g., (3+5)*2-1):\n");
    yyparse();
    return 0;
}

