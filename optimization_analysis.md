# Tutorial 11: Debugging Optimized Code

## Overview
This tutorial demonstrates how compiler optimizations affect assembly code and provides strategies for debugging optimized C++ libraries.

## Common Compiler Optimizations

### 1. Constant Folding and Propagation
```cpp
int constant_calculation() {
    int a = 10;
    int b = 20; 
    int c = 30;
    return a + b + c;  // Becomes: return 60;
}
```

**Unoptimized (-O0):**
```assembly
movl $10, -12(%rbp)    # Store a = 10
movl $20, -8(%rbp)     # Store b = 20
movl $30, -4(%rbp)     # Store c = 30
movl -12(%rbp), %edx   # Load a
movl -8(%rbp), %eax    # Load b
addl %eax, %edx        # a + b
movl -4(%rbp), %eax    # Load c
addl %edx, %eax        # (a + b) + c
```

**Optimized (-O2):**
```assembly
movl $60, %eax         # return 60; (computed at compile time)
ret
```

### 2. Function Inlining
Small functions are expanded inline to eliminate call overhead.

```cpp
inline int add_numbers(int x, int y) { return x + y; }
int use_inline_function(int base) {
    return add_numbers(add_numbers(base, 5), 10);
}
```

**Unoptimized:** Multiple function calls with stack operations
**Optimized:** Becomes `return base + 15;`

### 3. Loop Optimizations

#### Loop Unrolling
```cpp
void copy_array(const int* src, int* dest, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dest[i] = src[i];
    }
}
```

**Optimized version might unroll to:**
```assembly
# Process 4 elements at once
movq (%rsi), %rax      # Load 4 ints from source
movq %rax, (%rdi)      # Store 4 ints to dest
addq $16, %rsi         # Advance source by 16 bytes
addq $16, %rdi         # Advance dest by 16 bytes
```

#### Vectorization (SIMD)
```cpp
void multiply_array(std::vector<float>& data, float multiplier) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] *= multiplier;
    }
}
```

**Optimized with AVX:**
```assembly
vbroadcastss %xmm0, %ymm0   # Broadcast multiplier to 8 floats
vmulps (%rax), %ymm0, %ymm1 # Multiply 8 floats at once
vmovups %ymm1, (%rax)       # Store 8 results
```

### 4. Dead Code Elimination
Code that doesn't affect the program output is removed.

### 5. Register Allocation
Variables are kept in registers instead of memory when possible.

## Debugging Strategies for Optimized Code

### 1. Compare Optimization Levels
```bash
# Compile with different optimization levels
g++ -O0 -g tutorial11.cpp -o tutorial11_debug
g++ -O2 -g tutorial11.cpp -o tutorial11_optimized

# Compare assembly output
objdump -d tutorial11_debug > debug.asm
objdump -d tutorial11_optimized > optimized.asm
diff debug.asm optimized.asm
```

### 2. Use GDB with Optimized Code
```bash
gdb tutorial11_optimized
(gdb) set disassembly-flavor intel
(gdb) break main
(gdb) run
(gdb) disas simple_loop
```

**Challenges with optimized debugging:**
- Variables may be optimized away
- Execution order may be reordered
- Functions may be inlined
- Code may be vectorized

### 3. Recognize Optimization Patterns

#### Pattern: Strength Reduction
```cpp
int result = x * 8;  // Becomes: shl $3, %eax (left shift by 3)
```

#### Pattern: Branch Prediction Optimization
```assembly
# Compiler may reorder code to optimize branch prediction
test %eax, %eax
jz unlikely_case     # Jump to unlikely case
# Likely case code here (no jump needed)
jmp end
unlikely_case:
# Unlikely case code
end:
```

#### Pattern: Memory Access Optimization
```cpp
// Multiple array accesses might be combined
int sum = arr[i] + arr[i+1] + arr[i+2];

// Optimized to:
movq (%rax), %rdx      # Load 8 bytes (2 ints)
movl 8(%rax), %ecx     # Load third int
# Process with register operations
```

### 4. Debugging Techniques

#### Use Source-Level Debugging
```bash
(gdb) break simple_loop
(gdb) step    # May jump unexpectedly due to optimization
(gdb) print data.size()  # Variable may be optimized away
```

#### Examine Register Contents
```bash
(gdb) info registers
(gdb) print $rax
(gdb) x/8x $rsp    # Examine stack
```

#### Trace Execution Flow
```bash
(gdb) set logging on
(gdb) while 1
  > stepi
  > printf "RIP: %p\n", $rip
  > if $rip == 0x401234
    > quit
  > end
> end
```

## Compilation Instructions for Analysis

```bash
# Debug build (no optimization)
g++ -O0 -g -Wall tutorial11.cpp -o tutorial11_debug

# Release build (optimized)
g++ -O2 -g -Wall tutorial11.cpp -o tutorial11_release

# Aggressive optimization
g++ -O3 -g -Wall tutorial11.cpp -o tutorial11_aggressive

# Generate assembly listings
g++ -O0 -S tutorial11.cpp -o tutorial11_O0.s
g++ -O2 -S tutorial11.cpp -o tutorial11_O2.s
g++ -O3 -S tutorial11.cpp -o tutorial11_O3.s

# Compare assembly outputs
diff tutorial11_O0.s tutorial11_O2.s
```

## Key Takeaways

1. **Optimization changes code structure dramatically** - The assembly may bear little resemblance to the source
2. **Variables may disappear** - Compiler may eliminate or merge variables
3. **Function boundaries blur** - Inlining eliminates function calls
4. **Execution order changes** - Compiler reorders for efficiency
5. **Use multiple debugging approaches** - Combine source-level and assembly-level debugging
6. **Understand common patterns** - Learn to recognize optimized code patterns
7. **Compare optimization levels** - Use debug builds to understand optimized builds

## Real-World Application

When debugging production C++ libraries:
1. Always compare with debug builds when possible
2. Focus on data flow rather than exact instruction sequence
3. Use watchpoints on memory locations
4. Understand that optimized code may not map directly to source lines
5. Look for patterns like vectorization, loop unrolling, and inlining