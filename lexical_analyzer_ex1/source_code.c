#include <stdio.h>

int main() {
    int a = 10;
    float b = 20.5;
    char c = 'x';

    if (a < b) {
        a = a + 1;
    } else {
        b = b - 1.0;
    }

    return 0;
}
