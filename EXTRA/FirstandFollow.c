#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 10

char productions[MAX][20];
char first[26][100], follow[26][100];
int num;
char start;

int isNonTerminal(char symbol) {
    return isupper(symbol);
}

void addToSet(char *set, char val) {
    if (!strchr(set, val)) {
        int len = strlen(set);
        set[len] = val;
        set[len + 1] = '\0';
    }
}

void computeFirst(char symbol);
void computeFollow(char symbol);

// FIRST
void computeFirst(char symbol) {
    if (!isNonTerminal(symbol)) {
        addToSet(first[symbol - 'A'], symbol);
        return;
    }

    for (int i = 0; i < num; i++) {
        if (productions[i][0] == symbol) {
            char* rhs = productions[i] + 2; // skip LHS and '='
            if (rhs[0] == '#') {
                addToSet(first[symbol - 'A'], '#');
            } else {
                int j = 0;
                while (rhs[j]) {
                    char sym = rhs[j];
                    computeFirst(sym);
                    for (int k = 0; first[sym - 'A'][k]; k++) {
                        if (first[sym - 'A'][k] != '#') {
                            addToSet(first[symbol - 'A'], first[sym - 'A'][k]);
                        }
                    }
                    if (!strchr(first[sym - 'A'], '#')) break;
                    j++;
                    if (!rhs[j]) addToSet(first[symbol - 'A'], '#');
                }
            }
        }
    }
}

// FOLLOW
void computeFollow(char symbol) {
    if (symbol == start) {
        addToSet(follow[symbol - 'A'], '$'); // $ for end marker
    }

    for (int i = 0; i < num; i++) {
        char* rhs = productions[i] + 2;
        for (int j = 0; rhs[j]; j++) {
            if (rhs[j] == symbol) {
                if (rhs[j + 1]) {
                    char next = rhs[j + 1];
                    if (!isNonTerminal(next)) {
                        addToSet(follow[symbol - 'A'], next);
                    } else {
                        for (int k = 0; first[next - 'A'][k]; k++) {
                            if (first[next - 'A'][k] != '#') {
                                addToSet(follow[symbol - 'A'], first[next - 'A'][k]);
                            }
                        }
                        if (strchr(first[next - 'A'], '#')) {
                            computeFollow(productions[i][0]);
                            for (int k = 0; follow[productions[i][0] - 'A'][k]; k++) {
                                addToSet(follow[symbol - 'A'], follow[productions[i][0] - 'A'][k]);
                            }
                        }
                    }
                } else {
                    if (symbol != productions[i][0]) {
                        computeFollow(productions[i][0]);
                        for (int k = 0; follow[productions[i][0] - 'A'][k]; k++) {
                            addToSet(follow[symbol - 'A'], follow[productions[i][0] - 'A'][k]);
                        }
                    }
                }
            }
        }
    }
}

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &num);
    printf("Enter productions (E=TR) and use # for epsilon:\n");
    for (int i = 0; i < num; i++) {
        scanf("%s", productions[i]);
    }

    start = productions[0][0]; // First non-terminal is the start symbol

    for (int i = 0; i < num; i++) {
        computeFirst(productions[i][0]);
    }

    for (int i = 0; i < num; i++) {
        computeFollow(productions[i][0]);
    }

    printf("\nFirst sets:\n");
    for (int i = 0; i < 26; i++) {
        if (strlen(first[i]) > 0)
            printf("FIRST(%c) = { %s }\n", i + 'A', first[i]);
    }

    printf("\nFollow sets:\n");
    for (int i = 0; i < 26; i++) {
        if (strlen(follow[i]) > 0)
            printf("FOLLOW(%c) = { %s }\n", i + 'A', follow[i]);
    }

    return 0;
}


// Enter number of productions: 5
// Enter productions (E=TR) and use # for epsilon:
// S=AbCd
// A=a  
// A=Cf
// C=Ee
// E=h