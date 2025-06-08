#include <stdio.h>
#include <string.h>

void generateAssembly(char* tacLine, char* assemblyCode) {
    char var1[10], var2[10], var3[10], label[10];
    char op[3];

 
    if (sscanf(tacLine, "%s = %s %s %s", var1, var2, op, var3) == 4) {
   
        if (strcmp(op, "+") == 0) {
            sprintf(assemblyCode + strlen(assemblyCode), "MOV AX, %s        ; Load %s into AX\n", var2, var2);  // MOV AX, a
            sprintf(assemblyCode + strlen(assemblyCode), "ADD AX, %s        ; Add %s to AX\n", var3, var3);    // ADD AX, b
            sprintf(assemblyCode + strlen(assemblyCode), "MOV %s, AX        ; Move result into %s\n\n", var1, var1); // MOV t1, AX
        } 
        else if (strcmp(op, "*") == 0) {
            sprintf(assemblyCode + strlen(assemblyCode), "MOV AX, %s        ; Load %s into AX\n", var2, var2);  // MOV AX, t1
            sprintf(assemblyCode + strlen(assemblyCode), "MOV BX, %s        ; Load %s into BX\n", var3, var3);  // MOV BX, c
            sprintf(assemblyCode + strlen(assemblyCode), "MUL BX            ; Multiply AX by BX\n");
            sprintf(assemblyCode + strlen(assemblyCode), "MOV %s, AX        ; Store result in %s\n\n", var1, var1); // MOV t2, AX
        } 
        else if (strcmp(op, "-") == 0) {
            sprintf(assemblyCode + strlen(assemblyCode), "MOV AX, %s        ; Load %s into AX\n", var2, var2);  // MOV AX, t2
            sprintf(assemblyCode + strlen(assemblyCode), "SUB AX, %s        ; Subtract %s from AX\n", var3, var3); // SUB AX, 10
            sprintf(assemblyCode + strlen(assemblyCode), "MOV %s, AX        ; Store result in %s\n\n", var1, var1); // MOV d, AX
        }
    }
 
    else if (sscanf(tacLine, "if %s %s %s goto %s", var1, op, var2, label) == 4) {

        if (strcmp(op, "==") == 0) {
            sprintf(assemblyCode + strlen(assemblyCode), "MOV AX, %s        ; Load %s into AX\n", var1, var1);  // MOV AX, a
            sprintf(assemblyCode + strlen(assemblyCode), "CMP AX, %s        ; Compare AX with %s\n", var2, var2); // CMP AX, b
            sprintf(assemblyCode + strlen(assemblyCode), "JE L%s            ; Jump if Equal to label L%s\n\n", label, label); // JE L10
        }
        else if (strcmp(op, ">") == 0) {
            sprintf(assemblyCode + strlen(assemblyCode), "MOV AX, %s        ; Load %s into AX\n", var1, var1);  // MOV AX, c
            sprintf(assemblyCode + strlen(assemblyCode), "CMP AX, %s        ; Compare AX with %s\n", var2, var2); // CMP AX, 5
            sprintf(assemblyCode + strlen(assemblyCode), "JG L%s            ; Jump if Greater to label L%s\n\n", label, label); // JG L20
        }
        else if (strcmp(op, "<") == 0) {
            sprintf(assemblyCode + strlen(assemblyCode), "MOV AX, %s        ; Load %s into AX\n", var1, var1);  // MOV AX, c
            sprintf(assemblyCode + strlen(assemblyCode), "CMP AX, %s        ; Compare AX with %s\n", var2, var2); // CMP AX, 5
            sprintf(assemblyCode + strlen(assemblyCode), "JL L%s            ; Jump if Less to label L%s\n\n", label, label); // JL L20
        }
    }
    else if (sscanf(tacLine, "goto %s", label) == 1) {
        sprintf(assemblyCode + strlen(assemblyCode), "JMP L%s            ; Jump to label L%s\n\n", label, label);
    }
}

int main() {
    char tacCode[100];
    char assemblyCode[1000] = "";  

    printf("Enter Three-Address Code (TAC) with jumps:\n");

    while (fgets(tacCode, sizeof(tacCode), stdin)) {
        if (tacCode[0] == '\n') {
            break;
        }

        tacCode[strcspn(tacCode, "\n")] = 0;

        generateAssembly(tacCode, assemblyCode);
    }
    printf("\nGenerated Assembly Code:\n%s", assemblyCode);
    return 0;
}

