// sharedlib.c - Tutorial 14: Shared library for dynamic debugging
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Version information
#define LIB_VERSION "1.0.0"

// Internal helper function (not exported)
static int internal_calculate(int a, int b) {
    return (a * b) + (a + b);
}

// Exported function 1: Initialize library
__attribute__((visibility("default")))
int lib_initialize(void) {
    printf("[Library] Initializing version %s\n", LIB_VERSION);
    return 0;
}

// Exported function 2: Process data
__attribute__((visibility("default")))
int lib_process_data(const char* input, char* output, size_t output_size) {
    if (!input || !output || output_size == 0) {
        return -1;
    }
    
    size_t len = strlen(input);
    if (len >= output_size) {
        return -2;
    }
    
    // Simple transformation: reverse and uppercase
    for (size_t i = 0; i < len; i++) {
        char c = input[len - 1 - i];
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        output[i] = c;
    }
    output[len] = '\0';
    
    return len;
}

// Exported function 3: Callback mechanism
typedef int (*callback_func)(int);

__attribute__((visibility("default")))
int lib_execute_callback(callback_func cb, int value) {
    if (!cb) {
        return -1;
    }
    
    int internal_result = internal_calculate(value, 10);
    return cb(internal_result);
}

// Constructor/Destructor for library load/unload
__attribute__((constructor))
void lib_constructor(void) {
    printf("[Library] Loaded at %p\n", lib_constructor);
}

__attribute__((destructor))
void lib_destructor(void) {
    printf("[Library] Unloading\n");
}

// Global variable (for debugging practice)
__attribute__((visibility("default")))
int lib_global_counter = 0;

// Function that modifies global state
__attribute__((visibility("default")))
void lib_increment_counter(void) {
    lib_global_counter++;
}

// Compile with:
// gcc -shared -fPIC -o libshared14.so sharedlib.c
// Strip with: strip libshared14.so