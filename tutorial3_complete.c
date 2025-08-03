// tutorial3_complete.c
#include <stdio.h>

long factorial_asm(long n) {
    long result;
    
    __asm__ volatile (
        "movq $1, %%rax\n\t"        // result = 1
        "movq %1, %%rbx\n\t"        // counter = n
        "1:\n\t"                    // loop_start label
        "cmpq $1, %%rbx\n\t"       // compare counter with 1
        "jle 2f\n\t"               // if counter <= 1, jump to end
        "mulq %%rbx\n\t"           // result *= counter
        "decq %%rbx\n\t"           // counter--
        "jmp 1b\n\t"               // jump back to loop_start
        "2:\n\t"                   // loop_end label
        "movq %%rax, %0\n\t"       // store result
        : "=m" (result)
        : "m" (n)
        : "rax", "rbx", "rdx"      // mulq uses rdx:rax
    );
    
    return result;
}

int main() {
    long n = 5;
    printf("Factorial of %ld is: %ld\n", n, factorial_asm(n));  // Should print: Factorial of 5 is: 120
    return 0;
}