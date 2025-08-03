// tutorial7.c - Array processing and pointer arithmetic
#include <stdio.h>

// TODO: Implement array sum using assembly
long array_sum_asm(const long* arr, size_t count) {
    long sum;
    // TODO: Use assembly to:
    // 1. Load array pointer into RSI
    // 2. Load count into RCX
    // 3. Use loop to add each element to accumulator
    // 4. Use pointer arithmetic (add $8 to advance to next long)
    
    return sum;
}

// TODO: Implement linear search using assembly
int array_search_asm(const long* arr, size_t count, long target) {
    int index;
    // TODO: Use assembly to:
    // 1. Load array pointer, count, and target
    // 2. Loop through array comparing each element
    // 3. Return index if found, -1 if not found
    // 4. Use pointer arithmetic to advance through array
    
    return index;
}

// TODO: Implement find maximum element using assembly
long array_max_asm(const long* arr, size_t count) {
    long max_val;
    // TODO: Use assembly to:
    // 1. Load first element as initial max
    // 2. Loop through remaining elements
    // 3. Use conditional moves to update max
    // 4. Return maximum value found
    
    return max_val;
}

// TODO: Implement matrix element access (2D array)
long matrix_get_asm(const long* matrix, size_t rows, size_t cols, size_t row, size_t col) {
    long value;
    // TODO: Use assembly to:
    // 1. Calculate offset: (row * cols + col) * sizeof(long)
    // 2. Add offset to base pointer
    // 3. Load value from calculated address
    // Formula: matrix[row][col] = *(matrix + row * cols + col)
    
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
    
    printf("Sum: %ld\n", array_sum_asm(numbers, count));
    printf("Max: %ld\n", array_max_asm(numbers, count));
    printf("Search for 17: index %d\n", array_search_asm(numbers, count, 17));
    printf("Search for 99: index %d\n", array_search_asm(numbers, count, 99));
    printf("Matrix[1][2]: %ld\n", matrix_get_asm((long*)matrix, 3, 3, 1, 2));
    
    return 0;
}