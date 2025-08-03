// tutorial13.c - Reverse engineering a stripped binary
// This file creates a "mystery" program that will be stripped for analysis

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Obfuscated function names to make reverse engineering necessary
int a1b2c3(int x) {
    int result = x * x;
    if (result > 100) {
        result = result % 100;
    }
    return result + 42;
}

char* d4e5f6(const char* input) {
    int len = strlen(input);
    char* output = malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            output[i] = ((input[i] - 'a' + 13) % 26) + 'a';
        } else if (input[i] >= 'A' && input[i] <= 'Z') {
            output[i] = ((input[i] - 'A' + 13) % 26) + 'A';
        } else {
            output[i] = input[i];
        }
    }
    output[len] = '\0';
    return output;
}

struct MysteryStruct {
    int field1;
    int field2;
    char field3[16];
};

void g7h8i9(struct MysteryStruct* ms) {
    ms->field1 = a1b2c3(ms->field1);
    ms->field2 = ms->field1 ^ 0xDEADBEEF;
    strcpy(ms->field3, "PROCESSED");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <number> <string>\n", argv[0]);
        return 1;
    }
    
    int num = atoi(argv[1]);
    char* str = argv[2];
    
    // Process number
    int processed_num = a1b2c3(num);
    printf("Processed number: %d\n", processed_num);
    
    // Process string
    char* processed_str = d4e5f6(str);
    printf("Processed string: %s\n", processed_str);
    
    // Process structure
    struct MysteryStruct ms;
    ms.field1 = num;
    ms.field2 = 0;
    strcpy(ms.field3, "INITIAL");
    
    g7h8i9(&ms);
    printf("Structure: field1=%d, field2=%x, field3=%s\n", 
           ms.field1, ms.field2, ms.field3);
    
    free(processed_str);
    return 0;
}

// Compile this with:
// gcc -O2 -o mystery13 tutorial13.c
// strip mystery13
// Now debug the stripped binary!