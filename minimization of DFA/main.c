#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> // For memset, strcpy

#define MAX_STATES 20
#define MAX_ALPHABET_SIZE 5 // Max number of alphabet symbols (e.g., '0', '1', 'a', 'b')
#define MAX_PARTITIONS MAX_STATES // Max possible partitions is equal to max states

// Structure to represent a DFA
typedef struct {
    int num_states;
    int num_alphabet_symbols;
    char alphabet[MAX_ALPHABET_SIZE];
    int transitions[MAX_STATES][MAX_ALPHABET_SIZE]; // transitions[current_state][alphabet_idx] = next_state
    int start_state;
    bool is_final[MAX_STATES];
} DFA;

// Global array to store current partition ID for each original state
// state_partition_id[original_state_index] = partition_id
int state_partition_id[MAX_STATES];
int num_current_partitions; // Number of active partitions

// --- Function Prototypes ---
void get_dfa_input(DFA *dfa);
void print_dfa(const DFA *dfa);
void minimize_dfa(const DFA *original_dfa, DFA *minimized_dfa);
void print_current_partitions(const char *phase_description, int num_original_states);
int get_minimized_state_id(int original_state);
int get_alphabet_index(const DFA *dfa, char symbol);

// --- Main Function ---
int main() {
    DFA original_dfa;
    DFA minimized_dfa;

    printf("--- DFA Minimization Program ---\n");

    get_dfa_input(&original_dfa);
    printf("\n--- Original DFA ---\n");
    print_dfa(&original_dfa);

    minimize_dfa(&original_dfa, &minimized_dfa);

    printf("\n--- Minimized DFA ---\n");
    print_dfa(&minimized_dfa);

    return 0;
}

// --- Function Implementations ---

// Get DFA details from user
void get_dfa_input(DFA *dfa) {
    printf("Enter number of states (max %d): ", MAX_STATES);
    scanf("%d", &dfa->num_states);
    if (dfa->num_states <= 0 || dfa->num_states > MAX_STATES) {
        fprintf(stderr, "Invalid number of states.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter number of alphabet symbols (max %d): ", MAX_ALPHABET_SIZE);
    scanf("%d", &dfa->num_alphabet_symbols);
    if (dfa->num_alphabet_symbols <= 0 || dfa->num_alphabet_symbols > MAX_ALPHABET_SIZE) {
        fprintf(stderr, "Invalid number of alphabet symbols.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter alphabet symbols (e.g., 0 1): ");
    for (int i = 0; i < dfa->num_alphabet_symbols; i++) {
        scanf(" %c", &dfa->alphabet[i]); // Space before %c to consume newline
    }

    printf("Enter start state (0 to %d): ", dfa->num_states - 1);
    scanf("%d", &dfa->start_state);
    if (dfa->start_state < 0 || dfa->start_state >= dfa->num_states) {
        fprintf(stderr, "Invalid start state.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize all states as non-final
    memset(dfa->is_final, false, sizeof(dfa->is_final));
    printf("Enter final states (space separated, -1 to end): ");
    int final_state;
    while (scanf("%d", &final_state) == 1 && final_state != -1) {
        if (final_state < 0 || final_state >= dfa->num_states) {
            fprintf(stderr, "Invalid final state: %d. Ignoring.\n", final_state);
        } else {
            dfa->is_final[final_state] = true;
        }
    }

    printf("Enter transition table (next_state for each current_state and symbol):\n");
    printf("Format: Current_State Symbol Next_State\n");
    printf("Example: 0 0 1 (for state 0 on input '0' goes to state 1)\n");
    printf("Enter -1 for Current_State when done.\n");

    int current_state, next_state;
    char symbol_char;
    while (true) {
        printf("Transition: ");
        if (scanf("%d", &current_state) != 1 || current_state == -1) {
            break;
        }
        if (current_state < 0 || current_state >= dfa->num_states) {
            fprintf(stderr, "Invalid current state. Try again.\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }

        scanf(" %c", &symbol_char); // Space before %c to consume newline
        scanf("%d", &next_state);

        int alphabet_idx = get_alphabet_index(dfa, symbol_char);
        if (alphabet_idx == -1) {
            fprintf(stderr, "Invalid alphabet symbol '%c'. Try again.\n", symbol_char);
            continue;
        }
        if (next_state < 0 || next_state >= dfa->num_states) {
            fprintf(stderr, "Invalid next state. Try again.\n");
            continue;
        }
        dfa->transitions[current_state][alphabet_idx] = next_state;
    }
}

// Helper to get alphabet index from symbol character
int get_alphabet_index(const DFA *dfa, char symbol) {
    for (int i = 0; i < dfa->num_alphabet_symbols; i++) {
        if (dfa->alphabet[i] == symbol) {
            return i;
        }
    }
    return -1; // Not found
}

// Print DFA details
void print_dfa(const DFA *dfa) {
    printf("Number of states: %d\n", dfa->num_states);
    printf("Alphabet: ");
    for (int i = 0; i < dfa->num_alphabet_symbols; i++) {
        printf("'%c' ", dfa->alphabet[i]);
    }
    printf("\n");
    printf("Start state: Q%d\n", dfa->start_state);
    printf("Final states: { ");
    for (int i = 0; i < dfa->num_states; i++) {
        if (dfa->is_final[i]) {
            printf("Q%d ", i);
        }
    }
    printf("}\n");

    printf("Transition Table:\n");
    printf("%-7s", "State");
    for (int i = 0; i < dfa->num_alphabet_symbols; i++) {
        printf("| %-5c ", dfa->alphabet[i]);
    }
    printf("\n");
    printf("-------+");
    for (int i = 0; i < dfa->num_alphabet_symbols; i++) {
        printf("-------");
    }
    printf("\n");

    for (int i = 0; i < dfa->num_states; i++) {
        printf("Q%-6d", i);
        for (int j = 0; j < dfa->num_alphabet_symbols; j++) {
            printf("| Q%-5d ", dfa->transitions[i][j]);
        }
        printf("\n");
    }
}

// Print current partitioning of original states
void print_current_partitions(const char *phase_description, int num_original_states) {
    printf("\n--- %s ---\n", phase_description);
    printf("Current Partitions (Q_original -> P_id):\n");
    for (int p_id = 0; p_id < num_current_partitions; p_id++) {
        printf("P%d: { ", p_id);
        bool first_state_in_partition = true;
        for (int i = 0; i < num_original_states; i++) {
            if (state_partition_id[i] == p_id) {
                if (!first_state_in_partition) printf(", ");
                printf("Q%d", i);
                first_state_in_partition = false;
            }
        }
        printf(" }\n");
    }
}


// Main DFA Minimization Logic
void minimize_dfa(const DFA *original_dfa, DFA *minimized_dfa) {
    // Step 1: Initial Partition (P0)
    // Partition states into final (0) and non-final (1) sets
    num_current_partitions = 0;
    int final_partition_id = -1;
    int non_final_partition_id = -1;

    for (int i = 0; i < original_dfa->num_states; i++) {
        if (original_dfa->is_final[i]) {
            if (final_partition_id == -1) {
                final_partition_id = num_current_partitions++;
            }
            state_partition_id[i] = final_partition_id;
        } else {
            if (non_final_partition_id == -1) {
                non_final_partition_id = num_current_partitions++;
            }
            state_partition_id[i] = non_final_partition_id;
        }
    }
    // Handle case where all states are final or all are non-final
    if (final_partition_id == -1) final_partition_id = 0;
    if (non_final_partition_id == -1) non_final_partition_id = 0;


    print_current_partitions("Initial Partition (P0)", original_dfa->num_states);

    // Step 2: Iterative Refinement
    bool changed;
    int iteration = 0;
    do {
        changed = false;
        iteration++;
        printf("\n--- Refinement Iteration P%d ---\n", iteration);

        int new_partition_ids[MAX_STATES]; // Stores new partition IDs for this iteration
        memset(new_partition_ids, -1, sizeof(new_partition_ids)); // -1 means unassigned
        int next_new_partition_id = 0;

        // Iterate through each current partition set (from previous iteration)
        for (int p_id = 0; p_id < num_current_partitions; p_id++) {
            // Find all states belonging to this partition `p_id`
            int states_in_current_partition[MAX_STATES];
            int count_states_in_partition = 0;
            for (int i = 0; i < original_dfa->num_states; i++) {
                if (state_partition_id[i] == p_id) {
                    states_in_current_partition[count_states_in_partition++] = i;
                }
            }

            // Now, refine this group `states_in_current_partition`
            for (int i = 0; i < count_states_in_partition; i++) {
                int s1 = states_in_current_partition[i];

                // If s1 is already assigned a new partition ID, skip
                if (new_partition_ids[s1] != -1) continue;

                // Create a new partition for s1 and states equivalent to it
                int current_group_new_id = next_new_partition_id++;
                new_partition_ids[s1] = current_group_new_id;

                for (int j = i + 1; j < count_states_in_partition; j++) {
                    int s2 = states_in_current_partition[j];

                    // If s2 is already assigned, skip
                    if (new_partition_ids[s2] != -1) continue;

                    // Assume s1 and s2 are equivalent until proven otherwise
                    bool distinguishable = false;
                    for (int k = 0; k < original_dfa->num_alphabet_symbols; k++) {
                        int next_s1 = original_dfa->transitions[s1][k];
                        int next_s2 = original_dfa->transitions[s2][k];

                        // If their transitions lead to states in different *current* partitions, they are distinguishable
                        if (state_partition_id[next_s1] != state_partition_id[next_s2]) {
                            distinguishable = true;
                            break; // s1 and s2 are distinguishable on this symbol
                        }
                    }

                    if (!distinguishable) {
                        // s1 and s2 are indistinguishable in this iteration, assign to same new partition
                        new_partition_ids[s2] = current_group_new_id;
                    }
                }
            }
        }

        // Check if partitions have stabilized
        if (next_new_partition_id == num_current_partitions) {
            bool all_same = true;
            for (int i = 0; i < original_dfa->num_states; i++) {
                if (state_partition_id[i] != new_partition_ids[i]) {
                    all_same = false;
                    break;
                }
            }
            if (all_same) {
                changed = false; // No changes, partitions stabilized
            } else {
                changed = true; // Still changes, continue refining
            }
        } else {
            changed = true; // Number of partitions changed, so continue
        }

        // Update global partition IDs for next iteration
        for (int i = 0; i < original_dfa->num_states; i++) {
            state_partition_id[i] = new_partition_ids[i];
        }
        num_current_partitions = next_new_partition_id;

        print_current_partitions("Current Partitioning", original_dfa->num_states);

    } while (changed);

    printf("\n--- Partitioning Stabilized After %d Iterations ---\n", iteration);


    // Step 3: Construct the Minimized DFA
    minimized_dfa->num_states = num_current_partitions;
    minimized_dfa->num_alphabet_symbols = original_dfa->num_alphabet_symbols;
    memcpy(minimized_dfa->alphabet, original_dfa->alphabet, dfa->num_alphabet_symbols * sizeof(char)); // Copy alphabet

    // Determine new start state
    minimized_dfa->start_state = state_partition_id[original_dfa->start_state];

    // Determine new final states
    memset(minimized_dfa->is_final, false, sizeof(minimized_dfa->is_final));
    for (int i = 0; i < original_dfa->num_states; i++) {
        if (original_dfa->is_final[i]) {
            minimized_dfa->is_final[state_partition_id[i]] = true; // If any state in a partition was final, the new state is final
        }
    }

    // Determine new transitions
    for (int p_id = 0; p_id < num_current_partitions; p_id++) {
        // Pick an arbitrary state from this partition to represent it
        int representative_state = -1;
        for (int i = 0; i < original_dfa->num_states; i++) {
            if (state_partition_id[i] == p_id) {
                representative_state = i;
                break;
            }
        }

        for (int j = 0; j < original_dfa->num_alphabet_symbols; j++) {
            int original_next_state = original_dfa->transitions[representative_state][j];
            // The transition for the new state p_id on symbol j goes to the partition containing original_next_state
            minimized_dfa->transitions[p_id][j] = state_partition_id[original_next_state];
        }
    }
}
