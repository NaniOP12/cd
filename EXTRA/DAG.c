#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

typedef struct Node {
    char val[10];
    struct Node *left;
    struct Node *right;
} Node;

Node* stack[MAX];
int top = -1;

// Check if operator
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Precedence
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Push to stack
void push(Node* node) {
    stack[++top] = node;
}

// Pop from stack
Node* pop() {
    return stack[top--];
}

// Create new node
Node* createNode(char* val, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    strcpy(node->val, val);
    node->left = left;
    node->right = right;
    return node;
}

// Check for existing subexpression (reuse if match)
Node* findExisting(Node* list[], int count, char* op, Node* l, Node* r) {
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i]->val, op) == 0 &&
            list[i]->left == l && list[i]->right == r) {
            return list[i];
        }
    }
    return NULL;
}

// Convert infix to postfix
void infixToPostfix(char* infix, char postfix[][10], int* n) {
    char stack[MAX];
    int top = -1, j = 0;
    for (int i = 0; infix[i]; i++) {
        if (isspace(infix[i])) continue;

        if (isalnum(infix[i])) {
            char temp[10] = {0};
            int k = 0;
            while (isalnum(infix[i])) {
                temp[k++] = infix[i++];
            }
            i--;
            strcpy(postfix[j++], temp);
        }
        else if (infix[i] == '(') {
            stack[++top] = infix[i];
        }
        else if (infix[i] == ')') {
            while (top != -1 && stack[top] != '(') {
                char temp[10] = {stack[top--], '\0'};
                strcpy(postfix[j++], temp);
            }
            top--; // pop '('
        }
        else if (isOperator(infix[i])) {
            while (top != -1 && precedence(stack[top]) >= precedence(infix[i])) {
                char temp[10] = {stack[top--], '\0'};
                strcpy(postfix[j++], temp);
            }
            stack[++top] = infix[i];
        }
    }
    while (top != -1) {
        char temp[10] = {stack[top--], '\0'};
        strcpy(postfix[j++], temp);
    }
    *n = j;
}

// Postfix to DAG
Node* buildDAG(char postfix[][10], int n) {
    Node* reused[MAX];
    int reusedCount = 0;

    for (int i = 0; i < n; i++) {
        if (isOperator(postfix[i][0]) && strlen(postfix[i]) == 1) {
            Node* r = pop();
            Node* l = pop();

            Node* existing = findExisting(reused, reusedCount, postfix[i], l, r);
            if (existing) {
                push(existing);
            } else {
                Node* newNode = createNode(postfix[i], l, r);
                reused[reusedCount++] = newNode;
                push(newNode);
            }
        } else {
            push(createNode(postfix[i], NULL, NULL));
        }
    }
    return pop();
}

// Print DAG (in-order)
void printDAG(Node* root, int level) {
    if (!root) return;
    printDAG(root->right, level + 1);
    for (int i = 0; i < level; i++) printf("    ");
    printf("%s\n", root->val);
    printDAG(root->left, level + 1);
}

int main() {
    char expr[MAX];
    printf("Enter infix expression: ");
    fgets(expr, MAX, stdin);
    expr[strcspn(expr, "\n")] = 0; // remove newline

    char postfix[MAX][10];
    int n;

    infixToPostfix(expr, postfix, &n);

    printf("\nPostfix: ");
    for (int i = 0; i < n; i++) printf("%s ", postfix[i]);

    Node* dagRoot = buildDAG(postfix, n);

    printf("\n\nDAG (in visual form):\n");
    printDAG(dagRoot, 0);

    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// struct Node {
//     char op[5];      // Operator or operand
//     struct Node *left;
//     struct Node *right;
// };

// struct Node* createNode(char *op, struct Node* left, struct Node* right) {
//     struct Node* node = (struct Node*) malloc(sizeof(struct Node));
//     strcpy(node->op, op);
//     node->left = left;
//     node->right = right;
//     return node;
// }

// void printDAG(struct Node* root, int level) {
//     if (root == NULL) return;
//     printDAG(root->right, level + 1);
//     for (int i = 0; i < level; i++) printf("    ");
//     printf("%s\n", root->op);
//     printDAG(root->left, level + 1);
// }

// int main() {
//     // Construct (b - c)
//     struct Node* b = createNode("b", NULL, NULL);
//     struct Node* c = createNode("c", NULL, NULL);
//     struct Node* b_minus_c = createNode("-", b, c);

//     // a * (b - c)
//     struct Node* a1 = createNode("a", NULL, NULL);
//     struct Node* a_mul = createNode("*", a1, b_minus_c);

//     // a + (a * (b - c))
//     struct Node* a2 = createNode("a", NULL, NULL);
//     struct Node* left_add = createNode("+", a2, a_mul);

//     // Complete: (a + a*(b-c)) + (b - c)
//     struct Node* root = createNode("+", left_add, b_minus_c);

//     printf("DAG for Expression: a + a * (b - c) + (b - c)\n");
//     printDAG(root, 0);

//     return 0;
// }
