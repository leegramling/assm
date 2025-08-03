# Tutorial 14: Dynamic Library Debugging

## Overview
This tutorial covers debugging shared libraries (`.so` files on Linux, `.dylib` on macOS, `.dll` on Windows). You'll learn how to debug libraries that are loaded at runtime, handle symbol resolution, and trace execution across library boundaries.

## Tutorial Files
- `sharedlib.c` - Source for shared library
- `tutorial14_main.c` - Main program that loads the library
- `libshared14.so` - Compiled shared library (potentially stripped)

## Building the Tutorial

```bash
# Build the shared library
gcc -shared -fPIC -g -o libshared14.so sharedlib.c

# Build without debug symbols (stripped)
gcc -shared -fPIC -o libshared14.so sharedlib.c
strip libshared14.so

# Build the main program
gcc -g -o tutorial14 tutorial14_main.c -ldl

# Run the program
./tutorial14
```

## Part 1: Understanding Dynamic Loading

### How Dynamic Libraries Work

1. **Load Time**: Libraries loaded when program starts (linked with -l)
2. **Runtime**: Libraries loaded with dlopen() (our example)

### Key Functions
- `dlopen()` - Load library into memory
- `dlsym()` - Get symbol address from library
- `dlclose()` - Unload library
- `dlerror()` - Get error messages

## Part 2: Debugging Techniques

### 1. Breaking on Library Load

```bash
gdb ./tutorial14

# Break when library is loaded
(gdb) catch load
(gdb) catch load libshared14.so

# Or break on dlopen
(gdb) break dlopen
(gdb) run
```

### 2. Setting Breakpoints in Libraries

#### Before Library is Loaded
```bash
# Set pending breakpoint
(gdb) break lib_initialize
Function "lib_initialize" not defined.
Make breakpoint pending on future shared library load? (y or [n]) y

(gdb) info breakpoints
# Shows as <PENDING>
```

#### After Library is Loaded
```bash
# Find library base address
(gdb) info sharedlibrary
# or
(gdb) info proc mappings

# Set breakpoint at offset
(gdb) break *0x7ffff7bd1234  # Actual address after loading
```

### 3. Symbol Resolution

```bash
# Check if symbols are available
(gdb) info functions lib_*

# For stripped libraries, find exported symbols
(gdb) shell nm -D libshared14.so

# Or from within GDB
(gdb) info symbol 0x7ffff7bd1000
```

## Part 3: Advanced Debugging Scenarios

### Scenario 1: Debugging Without Symbols

```bash
# Find library load address
(gdb) info proc mappings | grep libshared

# Disassemble to find function boundaries
(gdb) x/100i 0x7ffff7bd1000

# Look for function prologues
(gdb) find /w 0x7ffff7bd1000, 0x7ffff7bd9000, 0xe5894855

# Set breakpoints on discovered functions
(gdb) break *0x7ffff7bd1234
```

### Scenario 2: Tracing Library Calls

```bash
# Python script for call tracing
(gdb) python
class LibraryTracer(gdb.Command):
    def __init__(self):
        super(LibraryTracer, self).__init__("trace-lib", gdb.COMMAND_USER)
        
    def invoke(self, arg, from_tty):
        # Set breakpoints on all PLT entries
        gdb.execute("break *0x401030")  # dlopen@plt
        gdb.execute("break *0x401040")  # dlsym@plt
        # etc...

LibraryTracer()
end

(gdb) trace-lib
```

### Scenario 3: Debugging Callbacks

```bash
# The library calls back into main program
# Set breakpoint in callback
(gdb) break my_callback

# When hit, examine the call stack
(gdb) bt
# Shows: my_callback <- lib_execute_callback <- main

# Examine how parameters are passed
(gdb) info args
(gdb) info registers
```

## Part 4: Platform-Specific Debugging

### Linux: Using LD_DEBUG

```bash
# Enable loader debugging
LD_DEBUG=all ./tutorial14 2>ld_debug.log

# Useful LD_DEBUG values:
# libs - library loading
# symbols - symbol binding
# bindings - symbol resolution
# all - everything

LD_DEBUG=libs,symbols ./tutorial14
```

### Finding Library Dependencies

```bash
# Static dependencies
ldd ./tutorial14
ldd ./libshared14.so

# Runtime dependencies
strace -e openat ./tutorial14 2>&1 | grep "\.so"
```

### Preloading Libraries

```bash
# Override library functions
LD_PRELOAD=./my_malloc.so ./tutorial14

# In GDB
(gdb) set environment LD_PRELOAD ./my_malloc.so
(gdb) run
```

## Part 5: Common Issues and Solutions

### Issue 1: Library Not Found

```bash
# Check library search path
(gdb) show environment LD_LIBRARY_PATH

# Set library path
(gdb) set environment LD_LIBRARY_PATH=/path/to/libs:$LD_LIBRARY_PATH
```

### Issue 2: Symbol Conflicts

```bash
# Check which library provides a symbol
(gdb) info symbol malloc
# Shows: malloc in section .text of /lib/x86_64-linux-gnu/libc.so.6

# Force specific library
(gdb) call (int)dlsym(handle, "function_name")
```

### Issue 3: Debugging Constructors/Destructors

```bash
# Library constructors run before main
(gdb) break _init
(gdb) break __libc_csu_init

# Set breakpoint on specific constructor
(gdb) info functions constructor
(gdb) break lib_constructor
```

## Part 6: Analysis Tools

### 1. Library Analysis

```bash
# Examine library structure
readelf -a libshared14.so

# Dynamic section
readelf -d libshared14.so

# Symbol versions
readelf -V libshared14.so

# Relocations
readelf -r libshared14.so
```

### 2. Runtime Analysis with ltrace

```bash
# Trace library calls
ltrace ./tutorial14

# Trace specific library
ltrace -l ./libshared14.so ./tutorial14

# With demangle for C++
ltrace -C ./tutorial14
```

### 3. Memory Maps

```bash
# In GDB
(gdb) info proc mappings

# From shell
cat /proc/$(pidof tutorial14)/maps

# Watch memory mapping changes
(gdb) watch $_inferior_memory_changed
```

## Part 7: Debugging Workflow

### Step-by-Step Process

1. **Initial Setup**
```bash
gdb ./tutorial14
(gdb) set stop-on-solib-events 1
(gdb) run
```

2. **Library Load Event**
```bash
# Stopped due to shared library event
(gdb) info sharedlibrary
(gdb) sharedlibrary libshared14.so  # Load symbols if available
```

3. **Set Breakpoints**
```bash
(gdb) break lib_process_data
(gdb) break *dlsym+0  # Break on symbol lookup
```

4. **Trace Execution**
```bash
(gdb) continue
# At breakpoint in library
(gdb) disassemble
(gdb) info args
(gdb) backtrace
```

## Part 8: Advanced Techniques

### Custom GDB Commands

```python
# Add to .gdbinit or load manually
python
class LibraryInfo(gdb.Command):
    def __init__(self):
        super(LibraryInfo, self).__init__("libinfo", gdb.COMMAND_USER)
        
    def invoke(self, arg, from_tty):
        # List all loaded libraries with addresses
        for obj in gdb.objfiles():
            if ".so" in obj.filename:
                print(f"{obj.filename}:")
                print(f"  Text: {hex(obj.minimal_symbols()[0].value().address)}")
                
LibraryInfo()
end
```

### Hooking Library Functions

```c
// interpose.c - Function interposition
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

int lib_process_data(const char* input, char* output, size_t size) {
    printf("[HOOK] lib_process_data called with: %s\n", input);
    
    // Call original
    int (*original)(const char*, char*, size_t);
    original = dlsym(RTLD_NEXT, "lib_process_data");
    
    int result = original(input, output, size);
    printf("[HOOK] lib_process_data returned: %d\n", result);
    
    return result;
}

// Compile: gcc -shared -fPIC -o interpose.so interpose.c -ldl
// Use: LD_PRELOAD=./interpose.so ./tutorial14
```

### Analyzing Stripped Libraries

```bash
# Find exported functions
objdump -T libshared14.so | grep " DF "

# Look for string references
strings -a libshared14.so

# Find function patterns
objdump -d libshared14.so | grep -A5 "push.*%rbp"

# Signature scanning
# Look for unique byte sequences
hexdump -C libshared14.so | grep "48 89 e5"
```

## Practice Exercises

1. **Exercise 1**: Find and call a hidden function in the library
2. **Exercise 2**: Modify the library's global variable from GDB
3. **Exercise 3**: Intercept and log all calls between main and library
4. **Exercise 4**: Debug the library without any symbols
5. **Exercise 5**: Create a tool to automatically find all functions in a stripped library

## Key Takeaways

1. **Dynamic libraries** add complexity but follow predictable patterns
2. **Symbol resolution** happens at runtime and can be intercepted
3. **Memory layout** changes with each run (ASLR) but relative offsets remain constant
4. **Debugging tools** like `LD_DEBUG` and `ltrace` complement GDB
5. **Function interposition** is powerful for understanding library behavior

## Additional Resources

- `man dlopen` - Dynamic loading documentation
- `man ld.so` - Dynamic linker documentation
- ELF specification for understanding library format
- Platform ABI documentation for calling conventions

Remember: Dynamic library debugging requires understanding both the static structure (ELF format) and runtime behavior (dynamic linking). Practice with increasingly complex scenarios to build expertise.