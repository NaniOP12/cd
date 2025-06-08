#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

void checkToken(char *str) {
    int i = 0;
    bool hasDecimal = false;

    printf("Initial State: q0\n");

    if (isalpha(str[i]) || str[i] == '_') {
        printf("Transition: q0 -> q1 (Letter/_ found: %c)\n", str[i]);
        i++;
        while (isalnum(str[i]) || str[i] == '_') {
            printf("Transition: q1 -> q2 (Letter/Digit/_ found: %c)\n", str[i]);
            i++;
        }
        printf("Final State: %s\n", str[i] == '\0' ? "q2 (ACCEPT - Identifier)" : "REJECT (Invalid Token)");
        return;
    }

    if (isdigit(str[i])) {
        printf("Transition: q0 -> q3 (Digit found: %c)\n", str[i]);
        i++;
        while (isdigit(str[i]) || (str[i] == '.' && !hasDecimal)) {
            if (str[i] == '.') {
                hasDecimal = true;
                printf("Transition: q3 -> q5 (Decimal point found)\n");
            } else {
                printf("Transition: q3 -> q4 (Digit found: %c)\n", str[i]);
            }
            i++;
        }
        printf("Final State: %s\n", (str[i] == '\0' && !(hasDecimal && str[i - 1] == '.')) ? "q4 (ACCEPT - Constant)" : "REJECT (Invalid Token)");
        return;
    }

    char *operators[] = {"+", "-", "*", "/", "%", "=", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "!"};
    int numOperators = sizeof(operators) / sizeof(operators[0]);

    for (int j = 0; j < numOperators; j++) {
        if (strcmp(str, operators[j]) == 0) {
            printf("Transition: q0 -> q5 (Operator found: %s)\n", str);
            printf("Final State: q6 (ACCEPT - Operator)\n");
            return;
        }
    }

    printf("Final State: REJECT (Invalid Token)\n");
}

int main() {
    char input[50];

    printf("Enter a string: ");
    scanf("%s", input);

    checkToken(input);

    return 0;
}