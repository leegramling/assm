// tutorial2_complete.c
#include <stdio.h>

int main() {
    long x = 15;
    long y = 25;
    long max_val;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load x into RAX
        "movq %2, %%rbx\n\t"        // Load y into RBX
        "cmpq %%rbx, %%rax\n\t"     // Compare RAX with RBX
        "cmovg %%rax, %%rcx\n\t"    // If RAX > RBX, move RAX to RCX
        "cmovle %%rbx, %%rcx\n\t"   // If RAX <= RBX, move RBX to RCX
        "movq %%rcx, %0\n\t"        // Store result
        : "=m" (max_val)
        : "m" (x), "m" (y)
        : "rax", "rbx", "rcx"
    );
    
    printf("Max of %ld and %ld is: %ld\n", x, y, max_val);  // Should print: Max of 15 and 25 is: 25
    return 0;
}