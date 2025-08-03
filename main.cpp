// main.cpp
#include <stdio.h>

extern int mysterious_function(int a, int b);

int main() {
    int result = mysterious_function(10, 15);
    printf("Result: %d\n", result);
    return 0;
}