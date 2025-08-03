// tutorial1.c - Basic register and memory operations
#include <stdio.h>

int main() {
    long a = 42;
    long b = 10;
    long result;
    
    // TODO: Use inline assembly to:
    // 1. Load 'a' into RAX
    // 2. Load 'b' into RBX  
    // 3. Add RAX and RBX, store in RCX
    // 4. Store RCX into 'result'
    
    printf("Result: %ld\n", result);
    return 0;
}