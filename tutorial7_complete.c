// tutorial7_complete.c - Array processing and pointer arithmetic
#include <stdio.h>

long array_sum_asm(const long* arr, size_t count) {
    long sum;
    
    __asm__ volatile (
        "xorq %%rax, %%rax\n\t"     // Clear accumulator (sum = 0)
        "movq %1, %%rsi\n\t"        // Load array pointer into RSI
        "movq %2, %%rcx\n\t"        // Load count into RCX
        "testq %%rcx, %%rcx\n\t"    // Check if count is zero
        "jz 2f\n\t"                 // If zero, skip to end
        "1:\n\t"                    // loop_start
        "addq (%%rsi), %%rax\n\t"   // Add current element to sum
        "addq $8, %%rsi\n\t"        // Advance pointer by 8 bytes (sizeof(long))
        "decq %%rcx\n\t"            // Decrement counter
        "jnz 1b\n\t"                // If not zero, loop back
        "2:\n\t"                    // loop_end
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (sum)
        : "m" (arr), "m" (count)
        : "rax", "rcx", "rsi"
    );
    
    return sum;
}

int array_search_asm(const long* arr, size_t count, long target) {
    int index;
    
    __asm__ volatile (
        "movq %1, %%rsi\n\t"        // Load array pointer
        "movq %2, %%rcx\n\t"        // Load count
        "movq %3, %%rdx\n\t"        // Load target value
        "xorq %%rax, %%rax\n\t"     // Clear index counter
        "testq %%rcx, %%rcx\n\t"    // Check if count is zero
        "jz 3f\n\t"                 // If zero, not found
        "1:\n\t"                    // loop_start
        "cmpq (%%rsi), %%rdx\n\t"   // Compare target with current element
        "je 2f\n\t"                 // If equal, found it
        "addq $8, %%rsi\n\t"        // Advance pointer
        "incq %%rax\n\t"            // Increment index
        "decq %%rcx\n\t"            // Decrement counter
        "jnz 1b\n\t"                // If not zero, continue loop
        "3:\n\t"                    // not_found
        "movq $-1, %%rax\n\t"       // Set result to -1 (not found)
        "2:\n\t"                    // found
        "movl %%eax, %0\n\t"        // Store result (convert to int)
        : "=m" (index)
        : "m" (arr), "m" (count), "m" (target)
        : "rax", "rcx", "rdx", "rsi"
    );
    
    return index;
}

long array_max_asm(const long* arr, size_t count) {
    long max_val;
    
    __asm__ volatile (
        "movq %1, %%rsi\n\t"        // Load array pointer
        "movq %2, %%rcx\n\t"        // Load count
        "testq %%rcx, %%rcx\n\t"    // Check if count is zero
        "jz 2f\n\t"                 // If zero, skip
        "movq (%%rsi), %%rax\n\t"   // Load first element as initial max
        "addq $8, %%rsi\n\t"        // Advance to second element
        "decq %%rcx\n\t"            // Decrement counter
        "jz 2f\n\t"                 // If only one element, done
        "1:\n\t"                    // loop_start
        "movq (%%rsi), %%rdx\n\t"   // Load current element
        "cmpq %%rax, %%rdx\n\t"     // Compare with current max
        "cmovg %%rdx, %%rax\n\t"    // If greater, update max
        "addq $8, %%rsi\n\t"        // Advance pointer
        "decq %%rcx\n\t"            // Decrement counter
        "jnz 1b\n\t"                // If not zero, continue
        "2:\n\t"                    // end
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (max_val)
        : "m" (arr), "m" (count)
        : "rax", "rcx", "rdx", "rsi"
    );
    
    return max_val;
}

long matrix_get_asm(const long* matrix, size_t rows, size_t cols, size_t row, size_t col) {
    long value;
    
    __asm__ volatile (
        "movq %1, %%rsi\n\t"        // Load matrix base pointer
        "movq %3, %%rax\n\t"        // Load row index
        "mulq %2\n\t"               // Multiply row * cols (result in RAX)
        "addq %4, %%rax\n\t"        // Add col index
        "leaq (%%rsi,%%rax,8), %%rsi\n\t"  // Calculate final address: base + index * 8
        "movq (%%rsi), %%rax\n\t"   // Load value from calculated address
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (value)
        : "m" (matrix), "m" (cols), "m" (row), "m" (col)
        : "rax", "rdx", "rsi"       // mulq uses rdx:rax
    );
    
    return value;
}

int main() {
    long numbers[] = {5, 12, 8, 3, 17, 9, 1, 15};
    size_t count = sizeof(numbers) / sizeof(numbers[0]);
    
    // Matrix: 3x3
    long matrix[3][3] = {
        {1, 2, 3},
        {4, 5, 6}, 
        {7, 8, 9}
    };
    
    printf("Array: ");
    for (size_t i = 0; i < count; i++) {
        printf("%ld ", numbers[i]);
    }
    printf("\n");
    
    printf("Sum: %ld\n", array_sum_asm(numbers, count));                    // Should print: 70
    printf("Max: %ld\n", array_max_asm(numbers, count));                    // Should print: 17
    printf("Search for 17: index %d\n", array_search_asm(numbers, count, 17));  // Should print: 4
    printf("Search for 99: index %d\n", array_search_asm(numbers, count, 99));  // Should print: -1
    printf("Matrix[1][2]: %ld\n", matrix_get_asm((long*)matrix, 3, 3, 1, 2));   // Should print: 6
    
    return 0;
}