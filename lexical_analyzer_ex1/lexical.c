#include <stdio.h>
#include <ctype.h>
#include <string.h>

const char *keywords[] = {
    "int", "return", "if", "else", "while", "for", "do", "break", "continue", "char", "float", "double", "void"
};
const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

int is_keyword(const char *str) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
void skip_whitespace_and_comments(FILE *fp) {
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (isspace(ch)) {
            continue;
        }
        if (ch == '/') {
            ch = fgetc(fp);
            if (ch == '/') {
                while ((ch = fgetc(fp)) != EOF && ch != '\n') {
                    continue;
                }
            } else if (ch == '*') { 
                while ((ch = fgetc(fp)) != EOF) {
                    if (ch == '*') {
                        ch = fgetc(fp);
                        if (ch == '/') {
                            break; 
                        }
                    }
                }
            } else {
                ungetc(ch, fp); 
                ungetc('/', fp); 
                break;
            }
        } else {
            ungetc(ch, fp); 
            break;
        }
    }
}

void lexical_analyzer(FILE *fp) {
    char ch;
    char buffer[100];
    int buffer_index = 0;
    int token_count = 0; 

    while ((ch = fgetc(fp)) != EOF) {
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
                printf("Keyword: %s\n", buffer);
            } else {
                printf("Identifier: %s\n", buffer);
            }
            token_count++;
        }
        else if (isdigit(ch)) {
            buffer_index = 0;
            while (isdigit(ch) || ch == '.') {
                buffer[buffer_index++] = ch;
                ch = fgetc(fp);
            }
            buffer[buffer_index] = '\0';
            ungetc(ch, fp); 
            printf("Number: %s\n", buffer);
            token_count++; 
        }
        else if (ispunct(ch)) {
            printf("Operator/Symbol: %c\n", ch);
            token_count++;
        }
    }

    printf("\nTotal number of tokens: %d\n", token_count);
}

int main() {
    FILE *fp = fopen("source_code.c", "r");
    if (fp == NULL) {
        printf("Error: Could not open file.\n");
        return 1;
    }

    printf("Lexical Analysis Output:\n");
    lexical_analyzer(fp);

    fclose(fp);
    return 0;   
}