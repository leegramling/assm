// tutorial2.c - Arithmetic operations and conditional execution
#include <stdio.h>

int main() {
    long x = 15;
    long y = 25;
    long max_val;
    
    // TODO: Use inline assembly to:
    // 1. Compare x and y
    // 2. Use conditional moves to store the larger value in max_val
    // Hint: Use cmpq and cmovg (conditional move if greater)
    
    printf("Max of %ld and %ld is: %ld\n", x, y, max_val);
    return 0;
}