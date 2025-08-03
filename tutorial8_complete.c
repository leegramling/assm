// tutorial8_complete.c - Bitwise operations and bit manipulation
#include <stdio.h>
#include <stdint.h>

int popcount_asm(uint64_t value) {
    int count;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load value into RAX
        "xorq %%rbx, %%rbx\n\t"     // Clear counter
        "testq %%rax, %%rax\n\t"    // Test if value is zero
        "jz 2f\n\t"                 // If zero, skip loop
        "1:\n\t"                    // loop_start
        "movq %%rax, %%rcx\n\t"     // Copy value
        "decq %%rcx\n\t"            // Subtract 1
        "andq %%rcx, %%rax\n\t"     // Clear lowest set bit: value &= (value - 1)
        "incq %%rbx\n\t"            // Increment counter
        "testq %%rax, %%rax\n\t"    // Test if any bits left
        "jnz 1b\n\t"                // If not zero, continue loop
        "2:\n\t"                    // end
        "movl %%ebx, %0\n\t"        // Store result
        : "=m" (count)
        : "m" (value)
        : "rax", "rbx", "rcx"
    );
    
    return count;
}

uint64_t extract_bits_asm(uint64_t value, int start_bit, int num_bits) {
    uint64_t result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load value
        "movl %2, %%ecx\n\t"        // Load start_bit into CL
        "shrq %%cl, %%rax\n\t"      // Shift right by start_bit positions
        "movl %3, %%ecx\n\t"        // Load num_bits
        "movq $1, %%rdx\n\t"        // Start with 1
        "shlq %%cl, %%rdx\n\t"      // Shift left to create 2^num_bits
        "decq %%rdx\n\t"            // Subtract 1 to create mask
        "andq %%rdx, %%rax\n\t"     // Apply mask
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (value), "m" (start_bit), "m" (num_bits)
        : "rax", "rcx", "rdx"
    );
    
    return result;
}

uint64_t set_bit_asm(uint64_t value, int bit_position) {
    uint64_t result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load value
        "movl %2, %%ecx\n\t"        // Load bit_position
        "movq $1, %%rdx\n\t"        // Load 1
        "shlq %%cl, %%rdx\n\t"      // Shift 1 left by bit_position
        "orq %%rdx, %%rax\n\t"      // OR with the mask to set bit
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (value), "m" (bit_position)
        : "rax", "rcx", "rdx"
    );
    
    return result;
}

uint64_t clear_bit_asm(uint64_t value, int bit_position) {
    uint64_t result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load value
        "movl %2, %%ecx\n\t"        // Load bit_position
        "movq $1, %%rdx\n\t"        // Load 1
        "shlq %%cl, %%rdx\n\t"      // Shift 1 left by bit_position
        "notq %%rdx\n\t"            // Invert all bits in mask
        "andq %%rdx, %%rax\n\t"     // AND with inverted mask to clear bit
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (value), "m" (bit_position)
        : "rax", "rcx", "rdx"
    );
    
    return result;
}

uint64_t rotate_left_asm(uint64_t value, int positions) {
    uint64_t result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load value
        "movl %2, %%ecx\n\t"        // Load positions
        "rolq %%cl, %%rax\n\t"      // Rotate left by CL positions
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (value), "m" (positions)
        : "rax", "rcx"
    );
    
    return result;
}

int is_power_of_2_asm(uint64_t value) {
    int result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load value
        "testq %%rax, %%rax\n\t"    // Test if value is zero
        "jz 2f\n\t"                 // If zero, not a power of 2
        "movq %%rax, %%rdx\n\t"     // Copy value
        "decq %%rdx\n\t"            // Subtract 1
        "andq %%rdx, %%rax\n\t"     // value & (value - 1)
        "setz %%al\n\t"             // Set AL to 1 if result is zero
        "movzbl %%al, %%eax\n\t"    // Zero-extend AL to EAX
        "jmp 3f\n\t"                // Jump to end
        "2:\n\t"                    // zero_case
        "xorl %%eax, %%eax\n\t"     // Return 0 for zero input
        "3:\n\t"                    // end
        "movl %%eax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (value)
        : "rax", "rdx"
    );
    
    return result;
}

void print_binary(uint64_t value) {
    for (int i = 63; i >= 0; i--) {
        printf("%c", (value & (1ULL << i)) ? '1' : '0');
        if (i % 8 == 0 && i > 0) printf(" ");
    }
    printf("\n");
}

int main() {
    uint64_t test_value = 0xDEADBEEFCAFEBABE;
    
    printf("Test value: 0x%lX\n", test_value);
    printf("Binary:     ");
    print_binary(test_value);
    
    printf("Population count: %d\n", popcount_asm(test_value));                    // Should count 1 bits
    
    printf("Extract bits 8-15: 0x%lX\n", extract_bits_asm(test_value, 8, 8));     // Should extract byte at position 8-15
    
    uint64_t modified = set_bit_asm(test_value, 0);
    printf("Set bit 0:        0x%lX\n", modified);                                 // Should set lowest bit
    
    modified = clear_bit_asm(test_value, 1);  
    printf("Clear bit 1:      0x%lX\n", modified);                                 // Should clear bit 1
    
    printf("Rotate left 4:    0x%lX\n", rotate_left_asm(test_value, 4));          // Should rotate bits left
    
    printf("Is 16 power of 2? %d\n", is_power_of_2_asm(16));                      // Should print: 1
    printf("Is 17 power of 2? %d\n", is_power_of_2_asm(17));                      // Should print: 0
    
    return 0;
}