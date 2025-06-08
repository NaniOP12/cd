%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);

typedef struct node {
    char type; // '+', '-', '*', '/', 'n' (number)
    int value; // only valid if type == 'n'
    struct node *left;
    struct node *right;
} Node;

Node *createNode(char type, int value, Node *left, Node *right);
void printTree(Node *root, int level);
void printExecutionOrder(Node *root);
int evaluate(Node *root);
void freeTree(Node *root);

// Global variable
Node *root;
%}

%union {
    int ival;          
    struct node *nptr;  
}

%token <ival> NUMBER
%type <nptr> expr

%left '+' '-'
%left '*' '/'
%right UMINUS

%%

input:
    expr { root = $1; }
    ;

expr:
      expr '+' expr { $$ = createNode('+', 0, $1, $3); }
    | expr '-' expr { $$ = createNode('-', 0, $1, $3); }
    | expr '*' expr { $$ = createNode('*', 0, $1, $3); }
    | expr '/' expr { $$ = createNode('/', 0, $1, $3); }
    | '-' expr %prec UMINUS { $$ = createNode('-', 0, createNode('n', 0, NULL, NULL), $2); }
    | '(' expr ')' { $$ = $2; }
    | NUMBER { $$ = createNode('n', $1, NULL, NULL); }
    ;

%%

Node *createNode(char type, int value, Node *left, Node *right) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->type = type;
    newNode->value = value;
    newNode->left = left;
    newNode->right = right;
    return newNode;
}

void printTree(Node *root, int level) {
    if (root == NULL) return;

    printTree(root->right, level + 1);

    for (int i = 0; i < level; i++) printf("    ");
    if (root->type == 'n')
        printf("%d\n", root->value);
    else
        printf("%c\n", root->type);

    printTree(root->left, level + 1);
}

void printExecutionOrder(Node *root) {
    if (root == NULL) return;

    printExecutionOrder(root->left);
    printExecutionOrder(root->right);

    if (root->type == 'n')
        printf("Push %d\n", root->value);
    else
        printf("Apply '%c'\n", root->type);
}

int evaluate(Node *root) {
    if (root == NULL) return 0;

    if (root->type == 'n') {
        return root->value;
    }

    int leftVal = evaluate(root->left);
    int rightVal = evaluate(root->right);

    switch (root->type) {
        case '+': return leftVal + rightVal;
        case '-': return leftVal - rightVal;
        case '*': return leftVal * rightVal;
        case '/':
            if (rightVal == 0) {
                yyerror("Division by zero");
                exit(1);
            }
            return leftVal / rightVal;
        default:
            yyerror("Unknown operator");
            exit(1);
    }
}

void freeTree(Node *root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(void) {
    printf("Enter an expression:\n");
    if (yyparse() == 0) {
        printf("\n=== Syntax Tree ===\n");
        printTree(root, 0);

        printf("\n=== Order of Execution ===\n");
        printExecutionOrder(root);

        int result = evaluate(root);
        printf("\n=== Final Result ===\n");
        printf("Result = %d\n", result);

        freeTree(root);
    }
    return 0;
}