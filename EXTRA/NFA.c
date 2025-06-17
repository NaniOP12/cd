#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define EPSILON 'e'

int state = 0;
int tcount = 0;
int transitions[MAX][3][MAX]; // transitions[state][0=a,1=b,2=epsilon][target states]
int trans_count[MAX][3] = {0};

typedef struct {
    int start, end;
} Fragment;

Fragment stack[MAX];
int top = -1;

void push(Fragment f) {
    stack[++top] = f;
}

Fragment pop() {
    return stack[top--];
}

void addTransition(int from, int to, char symbol) {
    int col = (symbol == 'a') ? 0 : (symbol == 'b') ? 1 : 2;
    transitions[from][col][trans_count[from][col]++] = to;
    if (from >= state) state = from + 1;
    if (to >= state) state = to + 1;
}

Fragment createSymbol(char symbol) {
    int s = state++;
    int e = state++;
    addTransition(s, e, symbol);
    Fragment f = {s, e};
    return f;
}

Fragment alternate(Fragment f1, Fragment f2) {
    int s = state++;
    int e = state++;
    addTransition(s, f1.start, EPSILON);
    addTransition(s, f2.start, EPSILON);
    addTransition(f1.end, e, EPSILON);
    addTransition(f2.end, e, EPSILON);
    Fragment f = {s, e};
    return f;
}

Fragment concat(Fragment f1, Fragment f2) {
    addTransition(f1.end, f2.start, EPSILON);
    Fragment f = {f1.start, f2.end};
    return f;
}

Fragment star(Fragment f1) {
    int s = state++;
    int e = state++;
    addTransition(s, f1.start, EPSILON);
    addTransition(f1.end, f1.start, EPSILON);
    addTransition(f1.end, e, EPSILON);
    addTransition(s, e, EPSILON);
    Fragment f = {s, e};
    return f;
}

void printENFATable(Fragment f) {
    printf("\nε-NFA Transition Table:\n");
    printf("%-5s %-10s %-10s %-10s\n", "State", "a", "b", "ε");

    for (int i = 0; i < state; i++) {
        printf("s%-4d ", i);
        for (int j = 0; j < 3; j++) {
            if (trans_count[i][j] == 0)
                printf("%-10s", "-");
            else {
                for (int k = 0; k < trans_count[i][j]; k++) {
                    printf("s%d", transitions[i][j][k]);
                    if (k != trans_count[i][j] - 1) printf(",");
                }
                printf("%*s", 10 - (2 * trans_count[i][j]), ""); // spacing
            }
        }
        printf("\n");
    }

    printf("\nStart State: s%d\n", f.start);
    printf("Final State: s%d\n", f.end);
}

int main() {
    char regex[MAX];
    printf("Enter postfix regular expression (a,b,|,.,* only) eg: ((a|b)*).a.b.b can be represented as ab|*abb  ");
    scanf("%s", regex);

    int len = strlen(regex);
    for (int i = 0; i < len; i++) {
        char ch = regex[i];
        if (ch == 'a' || ch == 'b') {
            push(createSymbol(ch));
        } else if (ch == '*') {
            Fragment f = pop();
            push(star(f));
        } else if (ch == '.') {
            Fragment f2 = pop();
            Fragment f1 = pop();
            push(concat(f1, f2));
        } else if (ch == '|') {
            Fragment f2 = pop();
            Fragment f1 = pop();
            push(alternate(f1, f2));
        } else {
            printf("Invalid character: %c\n", ch);
            return 1;
        }
    }

    Fragment final = pop();
    printENFATable(final);

    return 0;
}
