#include <stdio.h>
#include <stdlib.h>

void stackExample() {
    int a = 10;               
    int b = 20;               
    printf("\n[STACK] Address of a: %p", (void*)&a);
    printf("\n[STACK] Address of b: %p\n", (void*)&b);
}
void heapExample() {
    int *p = (int *)malloc(sizeof(int));   
    int *q = (int *)malloc(sizeof(int));   
    if (p == NULL || q == NULL) {
        printf("\n[HEAP] Memory allocation failed\n");
        return;
    }
    *p = 100;
    *q = 200;
    printf("\n[HEAP] Value at p: %d, Address: %p", *p, (void*)p);
    printf("\n[HEAP] Value at q: %d, Address: %p", *q, (void*)q);
    free(p);
    free(q);
    printf("\n[HEAP] Memory freed\n");
}

int main() {
    printf("Demonstrating Stack and Heap Management at Runtime:");
    printf("\n\n--- Stack Example ---");
    stackExample();
    printf("\n\n--- Heap Example ---");
    heapExample();
    return 0;
}
