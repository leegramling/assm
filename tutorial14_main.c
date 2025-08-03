// tutorial14_main.c - Main program that uses the shared library
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

// Function pointer types for dynamic loading
typedef int (*init_func)(void);
typedef int (*process_func)(const char*, char*, size_t);
typedef int (*callback_exec_func)(int (*)(int), int);
typedef void (*increment_func)(void);

// Callback function for the library
int my_callback(int value) {
    printf("[Main] Callback received value: %d\n", value);
    return value * 2;
}

int main(int argc, char* argv[]) {
    // Load library dynamically
    void* handle = dlopen("./libshared14.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        return 1;
    }
    
    printf("[Main] Library loaded successfully\n");
    
    // Get function pointers
    init_func lib_init = (init_func)dlsym(handle, "lib_initialize");
    process_func lib_process = (process_func)dlsym(handle, "lib_process_data");
    callback_exec_func lib_callback = (callback_exec_func)dlsym(handle, "lib_execute_callback");
    increment_func lib_increment = (increment_func)dlsym(handle, "lib_increment_counter");
    
    // Get global variable pointer
    int* counter_ptr = (int*)dlsym(handle, "lib_global_counter");
    
    if (!lib_init || !lib_process || !lib_callback || !lib_increment || !counter_ptr) {
        fprintf(stderr, "Failed to find symbols\n");
        dlclose(handle);
        return 1;
    }
    
    // Use the library
    printf("\n=== Testing Library Functions ===\n");
    
    // Initialize
    int ret = lib_init();
    printf("[Main] lib_initialize returned: %d\n", ret);
    
    // Process data
    const char* input = "Hello Dynamic Library";
    char output[256];
    ret = lib_process(input, output, sizeof(output));
    printf("[Main] lib_process_data returned: %d\n", ret);
    printf("[Main] Processed string: '%s'\n", output);
    
    // Test callback
    ret = lib_callback(my_callback, 7);
    printf("[Main] lib_execute_callback returned: %d\n", ret);
    
    // Test global variable
    printf("[Main] Initial counter value: %d\n", *counter_ptr);
    lib_increment();
    printf("[Main] Counter after increment: %d\n", *counter_ptr);
    
    // Unload library
    printf("\n[Main] Unloading library\n");
    dlclose(handle);
    
    return 0;
}

// Compile with:
// gcc -o tutorial14 tutorial14_main.c -ldl