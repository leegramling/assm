// tutorial8.c - Bitwise operations and bit manipulation
#include <stdio.h>
#include <stdint.h>

// TODO: Implement bit counting (population count) using assembly
int popcount_asm(uint64_t value) {
    int count;
    // TODO: Use assembly to:
    // 1. Count the number of 1 bits in the value
    // 2. Use a loop with bit shifting and testing
    // 3. Alternative: use popcnt instruction if available
    
    return count;
}

// TODO: Implement bit field extraction using assembly
uint64_t extract_bits_asm(uint64_t value, int start_bit, int num_bits) {
    uint64_t result;
    // TODO: Use assembly to:
    // 1. Shift value right by start_bit positions
    // 2. Create mask with num_bits set (2^num_bits - 1)
    // 3. AND the shifted value with the mask
    
    return result;
}

// TODO: Implement setting a specific bit using assembly
uint64_t set_bit_asm(uint64_t value, int bit_position) {
    uint64_t result;
    // TODO: Use assembly to:
    // 1. Create mask by shifting 1 left by bit_position
    // 2. OR the value with the mask to set the bit
    
    return result;
}

// TODO: Implement clearing a specific bit using assembly
uint64_t clear_bit_asm(uint64_t value, int bit_position) {
    uint64_t result;
    // TODO: Use assembly to:
    // 1. Create mask by shifting 1 left by bit_position
    // 2. NOT the mask to flip all bits
    // 3. AND the value with the inverted mask
    
    return result;
}

// TODO: Implement bit rotation left using assembly
uint64_t rotate_left_asm(uint64_t value, int positions) {
    uint64_t result;
    // TODO: Use assembly to:
    // 1. Use rol instruction to rotate bits left
    // 2. Handle case where positions >= 64
    
    return result;
}

// TODO: Implement fast power of 2 check using assembly
int is_power_of_2_asm(uint64_t value) {
    int result;
    // TODO: Use assembly to:
    // 1. Check if value is non-zero
    // 2. Use the trick: (value & (value - 1)) == 0
    // 3. Return 1 if power of 2, 0 otherwise
    
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
    
    printf("Population count: %d\n", popcount_asm(test_value));
    
    printf("Extract bits 8-15: 0x%lX\n", extract_bits_asm(test_value, 8, 8));
    
    uint64_t modified = set_bit_asm(test_value, 0);
    printf("Set bit 0:        0x%lX\n", modified);
    
    modified = clear_bit_asm(test_value, 1);
    printf("Clear bit 1:      0x%lX\n", modified);
    
    printf("Rotate left 4:    0x%lX\n", rotate_left_asm(test_value, 4));
    
    printf("Is 16 power of 2? %d\n", is_power_of_2_asm(16));
    printf("Is 17 power of 2? %d\n", is_power_of_2_asm(17));
    
    return 0;
}