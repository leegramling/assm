// tutorial4.c - Recursive function with proper stack management
#include <stdio.h>

// TODO: Implement fibonacci using assembly
// Follow x64 calling convention:
// - Argument in RDI
// - Return value in RAX
// - Preserve callee-saved registers
long fibonacci_asm(long n);

// Assembly implementation
__asm__(
    ".globl fibonacci_asm\n"
    "fibonacci_asm:\n"
    // TODO: Your implementation here
    // Handle base cases (n <= 1)
    // Recursive calls: fib(n-1) + fib(n-2)
    // Proper stack frame management
    "ret\n"
);

int main() {
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d) = %ld\n", i, fibonacci_asm(i));
    }
    return 0;
}