#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 100
#define MAX_LINE 100

char tac[MAX][MAX_LINE];
int total = 0;

int leaders[MAX] = {0}; 
int leader_index[MAX];
int basic_block_count = 0;

int label_to_index(const char* label) {
    int len = strlen(label);
    for (int i = 0; i < total; i++) {
        if (strncmp(tac[i], label, len) == 0 && tac[i][len] == ':')
            return i;
    }
    return -1;
}

void identify_leaders() {
    leaders[0] = 1; 

    for (int i = 0; i < total; i++) {
        if (strstr(tac[i], "goto")) {
            char *goto_ptr = strstr(tac[i], "goto");
            char label[20];
            sscanf(goto_ptr, "goto %s", label);
            int target_index = label_to_index(label);
            if (target_index != -1) {
                leaders[target_index] = 1; 
            }
            if (i + 1 < total)
                leaders[i + 1] = 1; 
        }
    }

    for (int i = 0; i < total; i++) {
        if (leaders[i]) {
            leader_index[basic_block_count++] = i;
        }
    }
}

void build_flow_graph() {
    printf("\nControl Flow Graph:\n");

    for (int i = 0; i < basic_block_count; i++) {
        int start = leader_index[i];
        int end = (i + 1 < basic_block_count) ? leader_index[i + 1] : total;
        printf("Block %d (Lines %d to %d):\n", i + 1, start + 1, end);
        for (int j = start; j < end; j++) {
            printf("  %s\n", tac[j]);
        }

        char *last = tac[end - 1];
        if (strstr(last, "goto")) {
            char label[20];
            sscanf(strstr(last, "goto"), "goto %s", label);
            int target = label_to_index(label);
            for (int k = 0; k < basic_block_count; k++) {
                if (leader_index[k] == target) {
                    printf("  => Block %d (jump to %s)\n", k + 1, label);
                }
            }
        } else if (strstr(last, "if")) {
            char label[20];
            sscanf(strstr(last, "goto"), "goto %s", label);
            int target = label_to_index(label);
            for (int k = 0; k < basic_block_count; k++) {
                if (leader_index[k] == target) {
                    printf("  => Block %d (conditional jump to %s)\n", k + 1, label);
                }
            }
            if (i + 1 < basic_block_count)
                printf("  => Block %d (fallthrough)\n", i + 2);
        } else {
            if (i + 1 < basic_block_count)
                printf("  => Block %d (fallthrough)\n", i + 2);
        }
    }
}

int main() {
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        perror("File open error");
        return 1;
    }

    while (fgets(tac[total], MAX_LINE, fp)) {
        tac[total][strcspn(tac[total], "\n")] = 0;
        total++;
    }
    fclose(fp);

    identify_leaders();
    build_flow_graph();

    return 0;
}

