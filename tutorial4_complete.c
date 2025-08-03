// tutorial4_complete.c
#include <stdio.h>

long fibonacci_asm(long n);

__asm__(
    ".globl fibonacci_asm\n"
    "fibonacci_asm:\n"
    "pushq %rbp\n\t"              // Save old base pointer
    "movq %rsp, %rbp\n\t"         // Set up new base pointer
    "pushq %rbx\n\t"              // Save callee-saved register
    "pushq %r12\n\t"              // Save another callee-saved register
    
    "cmpq $1, %rdi\n\t"           // Compare n with 1
    "jle base_case\n\t"           // If n <= 1, goto base_case
    
    // Recursive case: fib(n-1) + fib(n-2)
    "movq %rdi, %rbx\n\t"         // Save n in RBX
    "decq %rdi\n\t"               // n-1
    "call fibonacci_asm\n\t"       // Call fib(n-1)
    "movq %rax, %r12\n\t"         // Save fib(n-1) result in callee-saved R12
    
    "movq %rbx, %rdi\n\t"         // Restore n
    "subq $2, %rdi\n\t"           // n-2
    "call fibonacci_asm\n\t"       // Call fib(n-2)
    
    "addq %r12, %rax\n\t"         // fib(n-1) + fib(n-2)
    "jmp cleanup\n\t"
    
    "base_case:\n\t"
    "movq %rdi, %rax\n\t"         // Return n (0 or 1)
    
    "cleanup:\n\t"
    "popq %r12\n\t"               // Restore R12
    "popq %rbx\n\t"               // Restore RBX
    "popq %rbp\n\t"               // Restore base pointer
    "ret\n"
);

int main() {
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d) = %ld\n", i, fibonacci_asm(i));
    }
    return 0;
}