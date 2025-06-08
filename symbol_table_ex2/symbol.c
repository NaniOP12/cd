#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOLS 100

typedef struct {
    char name[50];
    char type[20];
    char kind[10];
    char scope[20];
    int address;
} SymbolEntry;

SymbolEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int currentAddress = 1000; // Starting simulated address

const char *keywords[] = {
    "int", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "float", "double", "void"
};
const char *library_functions[] = { "printf", "scanf", "main" };

const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);
const int num_library_functions = sizeof(library_functions) / sizeof(library_functions[0]);

int is_keyword(const char *str) {
    for (int i = 0; i < num_keywords; i++)
        if (strcmp(str, keywords[i]) == 0) return 1;
    return 0;
}

int is_library_function(const char *str) {
    for (int i = 0; i < num_library_functions; i++)
        if (strcmp(str, library_functions[i]) == 0) return 1;
    return 0;
}

void insertSymbol(const char *name, const char *type, const char *kind, const char *scope) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 &&
            strcmp(symbolTable[i].scope, scope) == 0 &&
            strcmp(symbolTable[i].kind, kind) == 0)
            return;
    }

    if (symbolCount >= MAX_SYMBOLS) return;

    strcpy(symbolTable[symbolCount].name, name);
    strcpy(symbolTable[symbolCount].type, type);
    strcpy(symbolTable[symbolCount].kind, kind);
    strcpy(symbolTable[symbolCount].scope, scope);
    symbolTable[symbolCount].address = currentAddress;
    currentAddress += 4; // simulate memory allocation (e.g., 4 bytes)
    symbolCount++;
}

void displaySymbolTable() {
    printf("\nSymbol Table:\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Name\t\tType\t\tKind\t\tScope\t\tAddress\n");
    printf("-----------------------------------------------------------------------------\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%s\t\t%s\t\t%s\t\t%s\t\t%d\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].kind,
               symbolTable[i].scope,
               symbolTable[i].address);
    }
    printf("-----------------------------------------------------------------------------\n");
}

void skip_whitespace_and_comments(FILE *fp) {
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (isspace(ch)) continue;

        if (ch == '/') {
            char next = fgetc(fp);
            if (next == '/') {
                while ((ch = fgetc(fp)) != EOF && ch != '\n');
            } else if (next == '*') {
                while ((ch = fgetc(fp)) != EOF) {
                    if (ch == '*' && fgetc(fp) == '/') break;
                }
            } else {
                ungetc(next, fp);
                ungetc(ch, fp);
                break;
            }
        } else {
            ungetc(ch, fp);
            break;
        }
    }
}

void skip_string_literal(FILE *fp) {
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\\') fgetc(fp);
        else if (ch == '"') break;
    }
}

void buildSymbolTable(FILE *fp) {
    char ch, buffer[100], last_type[20] = "";
    int buffer_index;
    char current_scope[20] = "global";

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '#') {
            while ((ch = fgetc(fp)) != EOF && ch != '\n');
            continue;
        }

        if (ch == '"') {
            skip_string_literal(fp);
            continue;
        }

        skip_whitespace_and_comments(fp);

        if (isalpha(ch) || ch == '_') {
            buffer_index = 0;
            while (isalnum(ch) || ch == '_') {
                buffer[buffer_index++] = ch;
                ch = fgetc(fp);
            }
            buffer[buffer_index] = '\0';
            ungetc(ch, fp);

            if (is_keyword(buffer)) {
                strcpy(last_type, buffer);
            } else if (!is_library_function(buffer)) {
                skip_whitespace_and_comments(fp);
                char next = fgetc(fp);
                if (next == '(') {
                    insertSymbol(buffer, last_type, "func", "global");
                    strcpy(current_scope, "local");
                    while ((ch = fgetc(fp)) != EOF && ch != ')');
                } else {
                    insertSymbol(buffer, last_type, "var", current_scope);
                    ungetc(next, fp);
                }
            }
        } else if (ch == '{') {
            strcpy(current_scope, "local");
        } else if (ch == '}') {
            strcpy(current_scope, "global");
        }
    }
}

int main() {
    FILE *fp = fopen("source_code.c", "r");
    if (!fp) {
        printf("Error: Cannot open source file.\n");
        return 1;
    }

    buildSymbolTable(fp);
    fclose(fp);
    displaySymbolTable();
    return 0;
}
