# x64 Assembly and GDB Tutorial

## Quick Refresher: x64 Assembly Essentials

### Registers Overview
- **RAX, RBX, RCX, RDX**: General purpose (64-bit)
- **EAX, EBX, ECX, EDX**: 32-bit versions (zero upper 32 bits)
- **AX, BX, CX, DX**: 16-bit versions
- **AL, BL, CL, DL**: 8-bit low versions
- **RSI, RDI**: Source/Destination index
- **RSP**: Stack pointer
- **RBP**: Base pointer
- **R8-R15**: Additional general purpose registers

### AT&T vs Intel Syntax
```assembly
# AT&T Syntax (GCC default)
movq $42, %rax          # Move immediate 42 into RAX

# Intel Syntax (more readable)
mov rax, 42             # Move immediate 42 into RAX
```

### x64 Calling Convention (System V ABI - Linux)
- **Arguments**: RDI, RSI, RDX, RCX, R8, R9 (then stack)
- **Return value**: RAX
- **Caller-saved**: RAX, RCX, RDX, RSI, RDI, R8-R11
- **Callee-saved**: RBX, RSP, RBP, R12-R15

### Common Instructions
- `mov dst, src`: Move data
- `add dst, src`: Add source to destination
- `sub dst, src`: Subtract source from destination
- `cmp a, b`: Compare (sets flags)
- `jmp label`: Unconditional jump
- `je/jz label`: Jump if equal/zero
- `jne/jnz label`: Jump if not equal/not zero
- `call func`: Function call
- `ret`: Return from function
- `push src`: Push onto stack
- `pop dst`: Pop from stack

## GDB Essentials for Assembly Debugging

### Key GDB Commands
```bash
# Compilation with debug info
gcc -g -o program program.c

# Start GDB
gdb ./program

# Set Intel syntax (more readable)
(gdb) set disassembly-flavor intel

# Show assembly
(gdb) disas main
(gdb) disas /m main    # With source intermixed

# Breakpoints
(gdb) break main
(gdb) break *0x401000  # Break at address

# Execution
(gdb) run
(gdb) stepi            # Step one instruction
(gdb) nexti            # Next instruction (don't step into calls)
(gdb) continue

# Examine registers
(gdb) info registers
(gdb) print $rax
(gdb) print/x $rax     # Hexadecimal format

# Examine memory
(gdb) x/4xw $rsp       # 4 words in hex from stack pointer
(gdb) x/s $rdi         # String at RDI
(gdb) x/i $rip         # Instruction at RIP

# Watch memory/registers
(gdb) watch $rax
(gdb) watch *(int*)0x601020
```

### Debugging C++ Libraries (Assembly Only)
When debugging stripped libraries:
```bash
# Find function addresses
(gdb) info functions
(gdb) disas 0x7ffff7a05000,0x7ffff7a05100

# Set breakpoints on addresses
(gdb) break *0x7ffff7a05010

# Trace execution
(gdb) set logging on
(gdb) while 1
  > stepi
  > printf "RIP: %p, RAX: %lx\n", $rip, $rax
  > if $rip == 0x7ffff7a05050
    > quit
  > end
> end
```

## IDE Setup

### VSCode Setup
Create `.vscode/launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Assembly",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/${fileBasenameNoExtension}",
            "args": [],
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Set Intel syntax",
                    "text": "set disassembly-flavor intel"
                }
            ]
        }
    ]
}
```

### CLion Setup
1. File → Settings → Build → Toolchains
2. Set debugger to GDB
3. Run → Edit Configurations → GDB options: `set disassembly-flavor intel`

---

# Tutorial 1: Basic Registers and Memory

## Problem
Write a program that demonstrates basic register operations and memory access.

### Your Task (Fill in the TODOs)
```c
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
```

### Completed Example
```c
// tutorial1_complete.c
#include <stdio.h>

int main() {
    long a = 42;
    long b = 10;
    long result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load a into RAX
        "movq %2, %%rbx\n\t"        // Load b into RBX
        "addq %%rbx, %%rax\n\t"     // Add RBX to RAX
        "movq %%rax, %0\n\t"        // Store RAX into result
        : "=m" (result)              // Output: result
        : "m" (a), "m" (b)          // Input: a, b
        : "rax", "rbx"              // Clobbered registers
    );
    
    printf("Result: %ld\n", result);  // Should print: Result: 52
    return 0;
}
```

**Compile and run:**
```bash
gcc -g -o tutorial1 tutorial1_complete.c
./tutorial1
```
**Expected output:** `Result: 52`

---

# Tutorial 2: Arithmetic and Flags

## Problem
Implement a program that performs arithmetic operations and demonstrates flag usage.

### Your Task
```c
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
```

### Completed Example
```c
// tutorial2_complete.c
#include <stdio.h>

int main() {
    long x = 15;
    long y = 25;
    long max_val;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load x into RAX
        "movq %2, %%rbx\n\t"        // Load y into RBX
        "cmpq %%rbx, %%rax\n\t"     // Compare RAX with RBX
        "cmovg %%rax, %%rcx\n\t"    // If RAX > RBX, move RAX to RCX
        "cmovle %%rbx, %%rcx\n\t"   // If RAX <= RBX, move RBX to RCX
        "movq %%rcx, %0\n\t"        // Store result
        : "=m" (max_val)
        : "m" (x), "m" (y)
        : "rax", "rbx", "rcx"
    );
    
    printf("Max of %ld and %ld is: %ld\n", x, y, max_val);  // Should print: Max of 15 and 25 is: 25
    return 0;
}
```

**Compile and run:**
```bash
gcc -g -o tutorial2 tutorial2_complete.c
./tutorial2
```
**Expected output:** `Max of 15 and 25 is: 25`

---

# Tutorial 3: Control Flow and Loops

## Problem
Implement a factorial function using assembly with loops and jumps.

### Your Task
```c
// tutorial3.c - Factorial using assembly loops
#include <stdio.h>

long factorial_asm(long n) {
    long result;
    // TODO: Implement factorial using assembly
    // 1. Initialize result to 1
    // 2. Use a loop to multiply result by decreasing values of n
    // 3. Use conditional jumps (jle, jmp)
    
    return result;
}

int main() {
    long n = 5;
    printf("Factorial of %ld is: %ld\n", n, factorial_asm(n));
    return 0;
}
```

### Completed Example
```c
// tutorial3_complete.c
#include <stdio.h>

long factorial_asm(long n) {
    long result;
    
    __asm__ volatile (
        "movq $1, %%rax\n\t"        // result = 1
        "movq %1, %%rbx\n\t"        // counter = n
        "1:\n\t"                    // loop_start label
        "cmpq $1, %%rbx\n\t"       // compare counter with 1
        "jle 2f\n\t"               // if counter <= 1, jump to end
        "mulq %%rbx\n\t"           // result *= counter
        "decq %%rbx\n\t"           // counter--
        "jmp 1b\n\t"               // jump back to loop_start
        "2:\n\t"                   // loop_end label
        "movq %%rax, %0\n\t"       // store result
        : "=m" (result)
        : "m" (n)
        : "rax", "rbx", "rdx"      // mulq uses rdx:rax
    );
    
    return result;
}

int main() {
    long n = 5;
    printf("Factorial of %ld is: %ld\n", n, factorial_asm(n));  // Should print: Factorial of 5 is: 120
    return 0;
}
```

**Compile and run:**
```bash
gcc -g -o tutorial3 tutorial3_complete.c
./tutorial3
```
**Expected output:** `Factorial of 5 is: 120`

---

# Tutorial 4: Function Calls and Stack

## Problem
Implement a recursive function in assembly that follows calling conventions.

### Your Task
```c
// tutorial4.c - Recursive function with proper stack management
#include <stdio.h>

// TODO: Implement fibonacci using assembly
// Follow x64 calling convention:
// - Argument in RDI
// - Return value in RAX
// - Preserve callee-saved registers
long fibonacci_asm(long n);

// Assembly implementation
__asm__(
    ".globl fibonacci_asm\n"
    "fibonacci_asm:\n"
    // TODO: Your implementation here
    // Handle base cases (n <= 1)
    // Recursive calls: fib(n-1) + fib(n-2)
    // Proper stack frame management
    "ret\n"
);

int main() {
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d) = %ld\n", i, fibonacci_asm(i));
    }
    return 0;
}
```

### Completed Example
```c
// tutorial4_complete.c
#include <stdio.h>

long fibonacci_asm(long n);

__asm__(
    ".globl fibonacci_asm\n"
    "fibonacci_asm:\n"
    "pushq %rbp\n\t"              // Save old base pointer
    "movq %rsp, %rbp\n\t"         // Set up new base pointer
    "pushq %rbx\n\t"              // Save callee-saved register
    "pushq %r12\n\t"              // Save another callee-saved register
    
    "cmpq $1, %rdi\n\t"           // Compare n with 1
    "jle base_case\n\t"           // If n <= 1, goto base_case
    
    // Recursive case: fib(n-1) + fib(n-2)
    "movq %rdi, %rbx\n\t"         // Save n in RBX
    "decq %rdi\n\t"               // n-1
    "call fibonacci_asm\n\t"       // Call fib(n-1)
    "movq %rax, %r12\n\t"         // Save fib(n-1) result in callee-saved R12
    
    "movq %rbx, %rdi\n\t"         // Restore n
    "subq $2, %rdi\n\t"           // n-2
    "call fibonacci_asm\n\t"       // Call fib(n-2)
    
    "addq %r12, %rax\n\t"         // fib(n-1) + fib(n-2)
    "jmp cleanup\n\t"
    
    "base_case:\n\t"
    "movq %rdi, %rax\n\t"         // Return n (0 or 1)
    
    "cleanup:\n\t"
    "popq %r12\n\t"               // Restore R12
    "popq %rbx\n\t"               // Restore RBX
    "popq %rbp\n\t"               // Restore base pointer
    "ret\n"
);

int main() {
    for (int i = 0; i <= 10; i++) {
        printf("fib(%d) = %ld\n", i, fibonacci_asm(i));
    }
    return 0;
}
```

**Compile and run:**
```bash
gcc -g -o tutorial4 tutorial4_complete.c
./tutorial4
```
**Expected output:**
```
fib(0) = 0
fib(1) = 1
fib(2) = 1
fib(3) = 2
fib(4) = 3
fib(5) = 5
fib(6) = 8
fib(7) = 13
fib(8) = 21
fib(9) = 34
fib(10) = 55
```

---

# Tutorial 5: Debugging C++ Libraries with GDB

## Problem
Debug a simple C++ "library" function using only assembly-level debugging.

### Setup
```cpp
// library.cpp - Compile as separate object
int mysterious_function(int a, int b) {
    int temp = a * 2;
    if (temp > b) {
        return temp + b;
    } else {
        return temp - b;
    }
}
```

```cpp
// main.cpp
#include <stdio.h>

extern int mysterious_function(int a, int b);

int main() {
    int result = mysterious_function(10, 15);
    printf("Result: %d\n", result);
    return 0;
}
```

### Your Task
1. Compile the library without debug symbols (simulating a stripped library)
2. Use GDB to trace through the execution at assembly level
3. Determine what the function does without looking at source

### Commands to run:
```bash
# Compile library without debug info (stripped)
g++ -c -O0 library.cpp -o library.o

# Compile main with debug info
g++ -g -O0 main.cpp library.o -o debug_example

# Debug with GDB
gdb ./debug_example
```

### GDB Session Example:
```bash
(gdb) set disassembly-flavor intel
(gdb) break main
(gdb) run
(gdb) disas mysterious_function
(gdb) break *<address_of_mysterious_function>
(gdb) continue
(gdb) stepi
(gdb) info registers
(gdb) x/4xw $rsp
```

This tutorial teaches you to analyze assembly code when source isn't available, which is crucial for debugging third-party libraries.

---

# Tutorial 6: String Operations and Memory Manipulation

## Problem
Implement efficient string operations using x64 string instructions.

### Skills Covered
- String scanning with `scasb`
- String copying with `lodsb`/`stosb` 
- String comparison with `cmpsb`
- Memory manipulation techniques

### Functions to Implement
```c
size_t strlen_asm(const char* str);     // Calculate string length
void strcpy_asm(char* dest, const char* src);  // Copy strings
int memcmp_asm(const void* ptr1, const void* ptr2, size_t num);  // Compare memory
```

**Key Assembly Instructions:**
- `scasb` - Scan string for byte in AL
- `lodsb` - Load byte from [RSI] into AL
- `stosb` - Store AL into [RDI]
- `cmpsb` - Compare bytes at [RSI] and [RDI]
- `cld` - Clear direction flag (forward)

**Expected Output:** String length: 14, String copy, Memory comparison results

---

# Tutorial 7: Array Processing and Pointer Arithmetic

## Problem
Process arrays efficiently using assembly with proper pointer arithmetic.

### Skills Covered
- Array traversal patterns
- Pointer arithmetic in assembly
- Matrix indexing calculations
- Search algorithms

### Functions to Implement
```c
long array_sum_asm(const long* arr, size_t count);
int array_search_asm(const long* arr, size_t count, long target);
long array_max_asm(const long* arr, size_t count);
long matrix_get_asm(const long* matrix, size_t rows, size_t cols, size_t row, size_t col);
```

**Key Concepts:**
- Pointer increment: `addq $8, %rsi` (for 8-byte longs)
- Matrix indexing: `(row * cols + col) * sizeof(element)`
- Conditional moves for comparisons

**Expected Output:** Array sum: 70, Max: 17, Search results, Matrix access

---

# Tutorial 8: Bitwise Operations and Bit Manipulation

## Problem
Implement efficient bit manipulation algorithms using x64 bit operations.

### Skills Covered
- Population count (number of 1 bits)
- Bit field extraction and manipulation
- Bit rotation operations
- Power-of-2 detection

### Functions to Implement
```c
int popcount_asm(uint64_t value);
uint64_t extract_bits_asm(uint64_t value, int start_bit, int num_bits);
uint64_t set_bit_asm(uint64_t value, int bit_position);
uint64_t rotate_left_asm(uint64_t value, int positions);
int is_power_of_2_asm(uint64_t value);
```

**Key Instructions:**
- `shl`/`shr` - Shift left/right
- `rol`/`ror` - Rotate left/right
- `and`/`or`/`xor` - Bitwise operations
- `bsf`/`bsr` - Bit scan forward/reverse

**Expected Output:** Bit manipulation results, power-of-2 detection

---

# Tutorial 9: Floating Point Operations with SSE

## Problem
Use SSE (Streaming SIMD Extensions) for efficient floating-point operations.

### Skills Covered
- SSE scalar operations (`addss`, `mulss`)
- SSE packed operations (`addps`, `mulps`)
- Vector processing (4 floats at once)
- SSE comparisons and conversions

### Functions to Implement
```c
float add_floats_sse(float a, float b);
void add_vectors_sse(const float* a, const float* b, float* result);
float dot_product_sse(const float* a, const float* b, int count);
float sqrt_sse(float value);
```

**Key Instructions:**
- `movss` - Move scalar single-precision
- `movups` - Move unaligned packed singles
- `addss`/`addps` - Add scalar/packed singles
- `mulss`/`mulps` - Multiply scalar/packed singles
- `sqrtss` - Square root scalar single

**Expected Output:** SSE arithmetic results, vector operations

---

# Tutorial 10: Mixed C++/Assembly Programming

## Problem
Integrate assembly code with C++ programs, handling calling conventions and object interactions.

### Skills Covered
- C++ to assembly function calls
- Assembly to C++ callbacks
- Object member access in assembly
- Template and class integration

### Components
- Assembly functions callable from C++
- C++ class methods using assembly
- Callback mechanisms
- Inline assembly in C++ methods

**Key Concepts:**
- System V ABI compliance
- `this` pointer handling
- Exception safety in mixed code

**Expected Output:** Mixed function call results, object interactions

---

# Tutorial 11: Debugging Optimized Code

## Problem
Understand and debug compiler-optimized assembly code.

### Skills Covered
- Recognizing optimization patterns
- Debugging with missing variables
- Understanding inlined functions
- Compiler transformation analysis

### Common Optimizations
- **Constant folding**: `a + b + c` becomes immediate value
- **Loop unrolling**: Multiple iterations per loop body
- **Function inlining**: Function calls eliminated
- **Dead code elimination**: Unused code removed
- **Register allocation**: Variables kept in registers

**Analysis Tools:**
```bash
gcc -O0 -S code.c -o unoptimized.s
gcc -O2 -S code.c -o optimized.s
diff unoptimized.s optimized.s
```

**Expected Output:** Performance comparisons, optimization analysis

---

# Tutorial 12: Understanding C++ Object Layout and VTables

## Problem
Analyze C++ object memory layout and virtual function call mechanisms.

### Skills Covered
- Object memory layout analysis
- VTable structure understanding
- Virtual function call tracing
- Multiple inheritance pointer adjustment

### Key Concepts
- **VTable pointer**: First 8 bytes of objects with virtual functions
- **Virtual calls**: Load vtable, index into vtable, call function
- **Multiple inheritance**: Multiple vtable pointers, pointer adjustment
- **RTTI**: Runtime type information storage

**Assembly Patterns:**
```assembly
# Virtual function call
mov    (%rdi),%rax        # Load vtable pointer
call   *0x8(%rax)         # Call vtable[1]

# Object construction
mov    $vtable,%rax       # Load vtable address
mov    %rax,(%rdi)        # Store in object
```

**Expected Output:** Object sizes, vtable analysis, inheritance behavior

---

# Tutorial 13: Reverse Engineering Stripped Binaries

## Problem
Analyze and understand a stripped binary with no debug symbols.

### Skills Covered
- Function boundary identification
- Algorithm reconstruction
- Pattern recognition in assembly
- Dynamic analysis techniques

### Analysis Approach
1. **Static Analysis**: Strings, entry points, call graphs
2. **Dynamic Analysis**: Execution tracing, input/output correlation
3. **Pattern Recognition**: Common compiler patterns, library calls
4. **Hypothesis Testing**: Predict behavior, verify with execution

**Tools Used:**
- GDB for dynamic analysis
- `strings` for literal extraction
- `objdump` for disassembly
- Pattern matching scripts

**Challenge:** Reverse engineer functions that implement:
- Number processing (quadratic with modulo)
- String transformation (ROT13 encoding)
- Structure manipulation

---

# Tutorial 14: Dynamic Library Debugging

## Problem
Debug shared libraries that are loaded at runtime using `dlopen()`.

### Skills Covered
- Dynamic loading mechanism understanding
- Symbol resolution debugging
- Library boundary tracing
- Runtime linking analysis

### Key Concepts
- **PLT/GOT**: Procedure Linkage Table and Global Offset Table
- **Symbol binding**: Lazy vs eager binding
- **Library constructors/destructors**
- **Memory mapping**: ASLR and relative addressing

### Debugging Techniques
```bash
# Break on library load
(gdb) catch load libexample.so

# Trace dynamic calls
(gdb) break dlopen
(gdb) break dlsym

# Library analysis
LD_DEBUG=all ./program
ltrace ./program
```

**Expected Output:** Dynamic loading traces, symbol resolution

---

## Advanced Debugging Resources

### Comprehensive Guides
- **debug.md** - Complete guide for debugging stripped C++ libraries
- **lldb.md** - LLDB debugging reference (alternative to GDB)
- **gdb_cheatsheet.md** - Quick reference for GDB commands

### Additional Tools
- **Makefile** - Build system for all tutorials
- **Tutorial analysis files** - Detailed guides for complex tutorials

## Next Steps

After completing these tutorials:
1. Practice with real C++ libraries
2. Learn to read compiler-generated assembly
3. Understand optimization effects on assembly
4. Practice with different calling conventions
5. Learn SIMD instructions (SSE, AVX)

## Useful GDB Scripts

Create `.gdbinit` in your project:
```bash
set disassembly-flavor intel
set print pretty on

define hook-stop
  info registers rax rbx rcx rdx rsi rdi rsp rbp
  x/4i $rip
end
```