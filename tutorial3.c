// tutorial3.c - Factorial using assembly loops
#include <stdio.h>

long factorial_asm(long n) {
    long result;
    // TODO: Implement factorial using assembly
    // 1. Initialize result to 1
    // 2. Use a loop to multiply result by decreasing values of n
    // 3. Use conditional jumps (jle, jmp)
    
    return result;
}

int main() {
    long n = 5;
    printf("Factorial of %ld is: %ld\n", n, factorial_asm(n));
    return 0;
}