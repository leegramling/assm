// tutorial6.c - String operations and memory manipulation
#include <stdio.h>
#include <string.h>

// TODO: Implement string length calculation using assembly
size_t strlen_asm(const char* str) {
    size_t len;
    // TODO: Use assembly to:
    // 1. Load string pointer into RSI
    // 2. Use scasb instruction to scan for null terminator
    // 3. Calculate length by subtracting original pointer
    // Hint: Use RCX as counter, set to -1 initially
    
    return len;
}

// TODO: Implement string copy using assembly  
void strcpy_asm(char* dest, const char* src) {
    // TODO: Use assembly to:
    // 1. Load dest into RDI, src into RSI
    // 2. Use lodsb/stosb or movsb to copy bytes
    // 3. Continue until null terminator is copied
}

// TODO: Implement memory comparison
int memcmp_asm(const void* ptr1, const void* ptr2, size_t num) {
    int result;
    // TODO: Use assembly to:
    // 1. Load pointers into RSI and RDI
    // 2. Use cmpsb instruction to compare bytes
    // 3. Return -1, 0, or 1 based on comparison
    
    return result;
}

int main() {
    char src[] = "Hello Assembly";
    char dest[50];
    char test1[] = "ABC";
    char test2[] = "ABD";
    
    printf("String length of '%s': %zu\n", src, strlen_asm(src));
    
    strcpy_asm(dest, src);
    printf("Copied string: '%s'\n", dest);
    
    printf("Comparing '%s' and '%s': %d\n", test1, test2, memcmp_asm(test1, test2, 3));
    
    return 0;
}