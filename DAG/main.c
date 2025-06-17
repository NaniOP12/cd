#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- DAG Node Types ---
typedef enum {
    NODE_VAR,       // Variable (e.g., 'a', 'b')
    NODE_OP         // Operation (e.g., '*', '+')
} NodeType;

// --- Operation Types ---
typedef enum {
    OP_MUL, // Multiplication
    OP_ADD  // Addition
    // Add more as needed: OP_SUB, OP_DIV, etc.
} OpType;

// Forward declaration
struct DAGNode;

// --- Node Data Union ---
typedef union {
    char var_name[2]; // For NODE_VAR (e.g., "a\0")
    struct {
        OpType op_type;
        struct DAGNode *left;
        struct DAGNode *right;
    } op; // For NODE_OP
} NodeData;

// --- DAG Node Structure ---
struct DAGNode {
    int id; // Unique ID for the node
    NodeType type;
    NodeData data;
    // For a real compiler, you might add:
    // - value (if it's a constant)
    // - reference count (how many times it's pointed to)
    // - type information (int, float, etc.)
};

// --- Global List of DAG Nodes (for simplicity, in a real compiler, this would be more sophisticated) ---
#define MAX_NODES 100
struct DAGNode *dag_nodes[MAX_NODES];
int next_node_id = 0; // Acts as a counter for unique IDs and next available index

// --- Helper function to create a new DAG node ---
struct DAGNode* create_node(NodeType type) {
    if (next_node_id >= MAX_NODES) {
        fprintf(stderr, "Error: Max DAG nodes reached.\n");
        exit(EXIT_FAILURE);
    }
    struct DAGNode *node = (struct DAGNode*)malloc(sizeof(struct DAGNode));
    if (!node) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    node->id = next_node_id;
    node->type = type;
    dag_nodes[next_node_id] = node; // Store in our global list
    next_node_id++;
    return node;
}

// --- Functions to create specific node types ---
struct DAGNode* get_var_node(char var) {
    // Check if variable node already exists
    for (int i = 0; i < next_node_id; i++) {
        if (dag_nodes[i]->type == NODE_VAR && dag_nodes[i]->data.var_name[0] == var) {
            return dag_nodes[i]; // Found existing variable node
        }
    }
    // If not found, create a new one
    struct DAGNode *node = create_node(NODE_VAR);
    node->data.var_name[0] = var;
    node->data.var_name[1] = '\0';
    return node;
}

struct DAGNode* get_op_node(OpType op_type, struct DAGNode *left, struct DAGNode *right) {
    // This is the core of CSE: Check if this exact operation already exists
    for (int i = 0; i < next_node_id; i++) {
        if (dag_nodes[i]->type == NODE_OP &&
            dag_nodes[i]->data.op.op_type == op_type &&
            dag_nodes[i]->data.op.left == left && // Compare pointers (same node)
            dag_nodes[i]->data.op.right == right) { // Compare pointers (same node)
            return dag_nodes[i]; // Found existing common subexpression
        }
    }
    // If not found, create a new one
    struct DAGNode *node = create_node(NODE_OP);
    node->data.op.op_type = op_type;
    node->data.op.left = left;
    node->data.op.right = right;
    return node;
}

// --- Function to print the DAG (for visualization) ---
void print_dag_node(struct DAGNode *node) {
    if (!node) return;

    printf("Node %d: ", node->id);
    if (node->type == NODE_VAR) {
        printf("VAR('%s')\n", node->data.var_name);
    } else if (node->type == NODE_OP) {
        char op_char;
        switch (node->data.op.op_type) {
            case OP_MUL: op_char = '*'; break;
            case OP_ADD: op_char = '+'; break;
            default: op_char = '?'; break;
        }
        printf("OP('%c', Left: Node %d, Right: Node %d)\n",
               op_char, node->data.op.left->id, node->data.op.right->id);
    }
}

void print_dag_summary() {
    printf("\n--- DAG Nodes Created ---\n");
    for (int i = 0; i < next_node_id; i++) {
        print_dag_node(dag_nodes[i]);
    }
    printf("-------------------------\n");
}

// --- Cleanup function ---
void free_dag() {
    for (int i = 0; i < next_node_id; i++) {
        free(dag_nodes[i]);
    }
    next_node_id = 0; // Reset for potential re-use
}

// --- Main function to demonstrate DAG construction for a*b+(a*b) ---
int main() {
    printf("Building DAG for: a*b + (a*b)\n");

    // 1. Get nodes for 'a' and 'b'
    struct DAGNode *node_a = get_var_node('a');
    struct DAGNode *node_b = get_var_node('b');

    // 2. Build node for 'a * b' (first occurrence)
    struct DAGNode *node_mul_ab_1 = get_op_node(OP_MUL, node_a, node_b);
    printf("Created/Reused node for 'a*b' (1st instance). Node ID: %d\n", node_mul_ab_1->id);


    // 3. Build node for 'a * b' (second occurrence)
    // This call to get_op_node will internally find and return node_mul_ab_1
    struct DAGNode *node_mul_ab_2 = get_op_node(OP_MUL, node_a, node_b);
    printf("Created/Reused node for 'a*b' (2nd instance). Node ID: %d\n", node_mul_ab_2->id);

    if (node_mul_ab_1 == node_mul_ab_2) {
        printf("Successfully reused the common subexpression 'a*b'!\n");
    } else {
        printf("Error: Common subexpression 'a*b' was not reused.\n");
    }

    // 4. Build node for the final addition: (a*b) + (a*b)
    struct DAGNode *node_add = get_op_node(OP_ADD, node_mul_ab_1, node_mul_ab_2); // Both point to the same node!
    printf("Created node for final addition '(a*b)+(a*b)'. Node ID: %d\n", node_add->id);


    // Print a summary of all nodes created in the DAG
    print_dag_summary();

    printf("\nOptimization insight: The multiplication 'a*b' is represented by a single node (Node %d).\n", node_mul_ab_1->id);
    printf("The final addition (Node %d) has both its left and right operands pointing to this single 'a*b' node.\n", node_add->id);
    printf("This means 'a*b' will only be computed once in the optimized code.\n");

    // Clean up memory
    free_dag();

    return 0;
}