// tutorial1_complete.c
#include <stdio.h>

int main() {
    long a = 42;
    long b = 10;
    long result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load a into RAX
        "movq %2, %%rbx\n\t"        // Load b into RBX
        "addq %%rbx, %%rax\n\t"     // Add RBX to RAX
        "movq %%rax, %0\n\t"        // Store RAX into result
        : "=m" (result)              // Output: result
        : "m" (a), "m" (b)          // Input: a, b
        : "rax", "rbx"              // Clobbered registers
    );
    
    printf("Result: %ld\n", result);  // Should print: Result: 52
    return 0;
}