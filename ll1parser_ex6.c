#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTIONS 20
#define MAX_SYMBOLS 50
#define MAX_NONTERMINALS 10
#define MAX_TERMINALS 20
#define MAX_TABLE_STRING 100

typedef struct {
    char lhs;
    char rhs[MAX_SYMBOLS];
} Production;

Production productions[MAX_PRODUCTIONS];
int productionCount = 0;

char nonTerminals[MAX_NONTERMINALS];
int nonTerminalCount = 0;

char terminals[MAX_TERMINALS];
int terminalCount = 0;

char firstSet[MAX_NONTERMINALS][MAX_TERMINALS];
int firstCount[MAX_NONTERMINALS];

char followSet[MAX_NONTERMINALS][MAX_TERMINALS];
int followCount[MAX_NONTERMINALS];

char parseTable[MAX_NONTERMINALS][MAX_TERMINALS+1][MAX_TABLE_STRING];

int inSet(char set[], int count, char symbol) {
    int i;
    for (i = 0; i < count; i++) {
        if (set[i] == symbol)
            return 1;
    }
    return 0;
}
void addToSet(char set[], int *count, char symbol) {
    if (!inSet(set, *count, symbol)) {
        set[(*count)++] = symbol;
    }
}
int getNonTerminalIndex(char nt) {
    int i;
    for (i = 0; i < nonTerminalCount; i++) {
        if (nonTerminals[i] == nt)
            return i;
    }
    return -1;
}
int getTerminalIndex(char t) {
    int i;
    for (i = 0; i < terminalCount; i++) {
        if (terminals[i] == t)
            return i;
    }
    return -1;
}
int isNonTerminal(char symbol) {
    return (symbol >= 'A' && symbol <= 'Z');
}
void addParseTableEntry(int row, int col, char *entry) {
    if (strlen(parseTable[row][col]) == 0) {
        strcpy(parseTable[row][col], entry);
    } else {
        strcat(parseTable[row][col], " | ");
        strcat(parseTable[row][col], entry);
    }
}
void computeFirstSets() {
    int changed = 1;
    while(changed) {
        changed = 0;
        int i, pos, k;
        for (i = 0; i < productionCount; i++) {
            char A = productions[i].lhs;
            int A_index = getNonTerminalIndex(A);
            char *alpha = productions[i].rhs;
            int addEpsilon = 1;
            for (pos = 0; pos < strlen(alpha) && addEpsilon; pos++) {
                char symbol = alpha[pos];
                if (symbol == ' ')
                    continue;
                if (!isNonTerminal(symbol)) {
                    if (symbol == '#' ) {
                        if (!inSet(firstSet[A_index], firstCount[A_index], '#')) {
                            addToSet(firstSet[A_index], &firstCount[A_index], '#');
                            changed = 1;
                        }
                    } else {
                        if (!inSet(firstSet[A_index], firstCount[A_index], symbol)) {
                            addToSet(firstSet[A_index], &firstCount[A_index], symbol);
                            changed = 1;
                        }
                    }
                    addEpsilon = 0;
                } else {
                    int symIndex = getNonTerminalIndex(symbol);
                    for (k = 0; k < firstCount[symIndex]; k++) {
                        char temp = firstSet[symIndex][k];
                        if (temp != '#' && !inSet(firstSet[A_index], firstCount[A_index], temp)) {
                            addToSet(firstSet[A_index], &firstCount[A_index], temp);
                            changed = 1;
                        }
                    }
                    if (inSet(firstSet[symIndex], firstCount[symIndex], '#'))
                        addEpsilon = 1;
                    else
                        addEpsilon = 0;
                }
            }
            if (addEpsilon) {
                if (!inSet(firstSet[A_index], firstCount[A_index], '#')) {
                    addToSet(firstSet[A_index], &firstCount[A_index], '#');
                    changed = 1;
                }
            }
        }
    }
}
void computeFollowSets() {
    int i, pos, j, k;
    int startIndex = getNonTerminalIndex(productions[0].lhs);
    addToSet(followSet[startIndex], &followCount[startIndex], '$');
   
    int changed = 1;
    while(changed) {
        changed = 0;
        for (i = 0; i < productionCount; i++) {
            char A = productions[i].lhs;
            char *alpha = productions[i].rhs;
            int len = strlen(alpha);
            for (pos = 0; pos < len; pos++) {
                char symbol = alpha[pos];
                if (isNonTerminal(symbol)) {
                    int symIndex = getNonTerminalIndex(symbol);
                    int followBefore = followCount[symIndex];    
                    int addFollowA = 0;
                    int epsilonInBeta = 1;
                    for (j = pos+1; j < len && epsilonInBeta; j++) {
                        char betaSymbol = alpha[j];
                        if (betaSymbol == ' ')
                            continue;
                        if (!isNonTerminal(betaSymbol)) {
                            addToSet(followSet[symIndex], &followCount[symIndex], betaSymbol);
                            epsilonInBeta = 0;
                        } else {
                            int betaIndex = getNonTerminalIndex(betaSymbol);
                            for (k = 0; k < firstCount[betaIndex]; k++) {
                                char temp = firstSet[betaIndex][k];
                                if (temp != '#' && !inSet(followSet[symIndex], followCount[symIndex], temp)) {
                                    addToSet(followSet[symIndex], &followCount[symIndex], temp);
                                }
                            }
                            if (inSet(firstSet[betaIndex], firstCount[betaIndex], '#'))
                                epsilonInBeta = 1;
                            else
                                epsilonInBeta = 0;
                        }
                    }
                    if (epsilonInBeta) {
                        int A_index = getNonTerminalIndex(A);
                        for (k = 0; k < followCount[A_index]; k++) {
                            char temp = followSet[A_index][k];
                            if (!inSet(followSet[symIndex], followCount[symIndex], temp)) {
                                addToSet(followSet[symIndex], &followCount[symIndex], temp);
                            }
                        }
                    }
                    if (followBefore != followCount[symIndex])
                        changed = 1;
                }
            }
        }
    }
}
void computeFirstOfString(char *str, char result[], int *resultCount) {
    int pos, k;
    *resultCount = 0;
    int epsilonAll = 1;
    for (pos = 0; pos < strlen(str) && epsilonAll; pos++) {
        char symbol = str[pos];
        if (symbol == ' ')
            continue;
        if (!isNonTerminal(symbol)) {
            addToSet(result, resultCount, symbol);
            epsilonAll = 0;
        } else {
            int index = getNonTerminalIndex(symbol);
            for (k = 0; k < firstCount[index]; k++) {
                char temp = firstSet[index][k];
                if (temp != '#' && !inSet(result, *resultCount, temp))
                    addToSet(result, resultCount, temp);
            }
            if (inSet(firstSet[index], firstCount[index], '#'))
                epsilonAll = 1;
            else
                epsilonAll = 0;
        }
    }
    if (epsilonAll)
        addToSet(result, resultCount, '#');
}
void buildParseTable() {
    int i, j, k;
    for (i = 0; i < nonTerminalCount; i++) {
        for (j = 0; j < terminalCount+1; j++) {
            parseTable[i][j][0] = '\0';
        }
    }
    for (i = 0; i < productionCount; i++) {
        char A = productions[i].lhs;
        int row = getNonTerminalIndex(A);
       
        char firstAlpha[MAX_TERMINALS];
        int firstAlphaCount = 0;
        computeFirstOfString(productions[i].rhs, firstAlpha, &firstAlphaCount);
        for (k = 0; k < firstAlphaCount; k++) {
            if (firstAlpha[k] != '#') {
                int col = getTerminalIndex(firstAlpha[k]);
                if(col == -1) continue;
                {
                    char prodStr[MAX_TABLE_STRING];
                    sprintf(prodStr, "%c->%s", A, productions[i].rhs);
                    addParseTableEntry(row, col, prodStr);
                }
            }
        }
        if (inSet(firstAlpha, firstAlphaCount, '#')) {
            for (k = 0; k < followCount[row]; k++) {
                char b = followSet[row][k];
                int col;
                if (b == '$')
                    col = terminalCount;
                else
                    col = getTerminalIndex(b);
                if(col == -1) continue;
                {
                    char prodStr[MAX_TABLE_STRING];
                    sprintf(prodStr, "%c->%s", A, productions[i].rhs);
                    addParseTableEntry(row, col, prodStr);
                }
            }
        }
    }
}
void printParseTable() {
    int i, j;
    printf("\nLL(1) Parse Table:\n");
    printf("\t");
    for (i = 0; i < terminalCount; i++) {
        printf("%c\t", terminals[i]);
    }
    printf("$\t\n");
    for (i = 0; i < nonTerminalCount; i++) {
        printf("%c\t", nonTerminals[i]);
        for (j = 0; j < terminalCount+1; j++) {
            if (strlen(parseTable[i][j]) > 0)
                printf("%s\t", parseTable[i][j]);
            else
                printf("-\t");
        }
        printf("\n");
    }
}
void printFirstSets() {
    int i, j;
    printf("\nFIRST sets:\n");
    for (i = 0; i < nonTerminalCount; i++) {
        printf("FIRST(%c) = { ", nonTerminals[i]);
        for (j = 0; j < firstCount[i]; j++) {
            printf("%c ", firstSet[i][j]);
        }
        printf("}\n");
    }
}
void printFollowSets() {
    int i, j;
    printf("\nFOLLOW sets:\n");
    for (i = 0; i < nonTerminalCount; i++) {
        printf("FOLLOW(%c) = { ", nonTerminals[i]);
        for (j = 0; j < followCount[i]; j++) {
            printf("%c ", followSet[i][j]);
        }
        printf("}\n");
    }
}
void processSymbols() {
    int i, j;
    for (i = 0; i < productionCount; i++) {
        char A = productions[i].lhs;
        if (getNonTerminalIndex(A) == -1)
            nonTerminals[nonTerminalCount++] = A;
    }
    for (i = 0; i < productionCount; i++) {
        char *rhs = productions[i].rhs;
        for (j = 0; j < strlen(rhs); j++) {
            char symbol = rhs[j];
            if (symbol == ' ' || symbol == '\n') continue;
            if (!isNonTerminal(symbol)) {
                if (symbol != '#' && getTerminalIndex(symbol) == -1)
                    terminals[terminalCount++] = symbol;
            }
        }
    }
}

int main() {
    int i;
    printf("Enter the number of productions: ");
    scanf("%d", &productionCount);
    getchar();
    printf("Enter each production in the form A=alpha (use '#' for epsilon):\n");
    for (i = 0; i < productionCount; i++) {
        char inputLine[100];
        fgets(inputLine, sizeof(inputLine), stdin);
        inputLine[strcspn(inputLine, "\n")] = '\0';
        if (strlen(inputLine) < 3 || inputLine[1] != '=') {
            printf("Invalid production format.\n");
            i--;
            continue;
        }
        productions[i].lhs = inputLine[0];
        strcpy(productions[i].rhs, inputLine+2);
    }
    processSymbols();
    for (i = 0; i < MAX_NONTERMINALS; i++) {
        firstCount[i] = 0;
        followCount[i] = 0;
    }
    computeFirstSets();
    computeFollowSets();
    buildParseTable();
    printFirstSets();
    printFollowSets();
    printParseTable();
   
    return 0;
}