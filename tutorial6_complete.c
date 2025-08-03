// tutorial6_complete.c - String operations and memory manipulation
#include <stdio.h>
#include <string.h>

size_t strlen_asm(const char* str) {
    size_t len;
    
    __asm__ volatile (
        "movq %1, %%rdi\n\t"        // Load string pointer into RDI
        "xorq %%rax, %%rax\n\t"     // Clear RAX (will be our counter)
        "movq $-1, %%rcx\n\t"       // Set RCX to -1 (maximum count)
        "cld\n\t"                   // Clear direction flag (forward)
        "repne scasb\n\t"           // Scan for null byte (AL = 0)
        "notq %%rcx\n\t"            // Invert RCX
        "decq %%rcx\n\t"            // Subtract 1 (don't count null terminator)
        "movq %%rcx, %0\n\t"        // Store result
        : "=m" (len)
        : "m" (str)
        : "rax", "rcx", "rdi"
    );
    
    return len;
}

void strcpy_asm(char* dest, const char* src) {
    __asm__ volatile (
        "movq %0, %%rdi\n\t"        // Load dest into RDI
        "movq %1, %%rsi\n\t"        // Load src into RSI
        "cld\n\t"                   // Clear direction flag
        "1:\n\t"                    // loop_start
        "lodsb\n\t"                 // Load byte from [RSI] into AL, increment RSI
        "stosb\n\t"                 // Store AL into [RDI], increment RDI
        "testb %%al, %%al\n\t"      // Test if AL is zero (null terminator)
        "jnz 1b\n\t"                // If not zero, loop back
        :
        : "m" (dest), "m" (src)
        : "rax", "rsi", "rdi"
    );
}

int memcmp_asm(const void* ptr1, const void* ptr2, size_t num) {
    int result;
    
    __asm__ volatile (
        "movq %1, %%rsi\n\t"        // Load ptr1 into RSI
        "movq %2, %%rdi\n\t"        // Load ptr2 into RDI  
        "movq %3, %%rcx\n\t"        // Load num into RCX
        "cld\n\t"                   // Clear direction flag
        "repe cmpsb\n\t"            // Compare bytes while equal
        "movl $0, %%eax\n\t"        // Default result = 0 (equal)
        "je 2f\n\t"                 // If equal, skip to end
        "movl $-1, %%eax\n\t"       // Assume ptr1 < ptr2
        "jb 2f\n\t"                 // If below, result = -1
        "movl $1, %%eax\n\t"        // Otherwise ptr1 > ptr2, result = 1
        "2:\n\t"                    // end
        "movl %%eax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (ptr1), "m" (ptr2), "m" (num)
        : "rax", "rcx", "rsi", "rdi"
    );
    
    return result;
}

int main() {
    char src[] = "Hello Assembly";
    char dest[50];
    char test1[] = "ABC";
    char test2[] = "ABD";
    
    printf("String length of '%s': %zu\n", src, strlen_asm(src));           // Should print: 14
    
    strcpy_asm(dest, src);
    printf("Copied string: '%s'\n", dest);                                  // Should print: Hello Assembly
    
    printf("Comparing '%s' and '%s': %d\n", test1, test2, memcmp_asm(test1, test2, 3));  // Should print: -1
    
    return 0;
}