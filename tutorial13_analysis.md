# Tutorial 13: Reverse Engineering Stripped Binaries

## Overview
This tutorial teaches you how to analyze and understand a stripped binary with no debug symbols. We'll work through identifying functions, understanding their behavior, and reconstructing the program logic.

## The Challenge
You have a stripped binary `mystery13` that takes two command-line arguments. Your task is to:
1. Identify the main functions
2. Understand what each function does
3. Reconstruct the program logic
4. Document the algorithm

## Step 1: Initial Analysis

### Gather Basic Information
```bash
# Check file type
file mystery13

# Look for strings
strings mystery13 | grep -E "Usage|Processed|INITIAL"

# Check symbols (minimal in stripped binary)
nm mystery13

# Get section information
objdump -h mystery13
```

### Expected Findings
- Usage string reveals expected arguments: `<number> <string>`
- Output strings: "Processed number:", "Processed string:", "Structure:"
- Some literal strings: "INITIAL", "PROCESSED"

## Step 2: Dynamic Analysis with GDB

### Initial Exploration
```bash
gdb ./mystery13

# Set Intel syntax
(gdb) set disassembly-flavor intel

# Run with test arguments
(gdb) run 10 "hello"

# Set breakpoint at main (found via entry point analysis)
(gdb) info files
# Note entry point, then trace to main

(gdb) break *<main_address>
(gdb) run 10 "hello"
```

### Finding Main Function
```bash
# The entry point calls __libc_start_main with main as first argument
(gdb) x/10i <entry_point>

# Look for pattern:
# mov rdi, <main_address>
# call __libc_start_main
```

## Step 3: Function Identification

### Technique 1: Call Graph Analysis
```bash
# At main, examine all call instructions
(gdb) disas main
# Note all 'call' instructions to internal functions

# For each called function:
(gdb) disas <function_address>
```

### Technique 2: String Reference Tracking
```bash
# Find where strings are referenced
(gdb) find /s 0x400000, 0x500000, "Processed number"

# Examine code around string references
(gdb) x/20i <address_before_string_ref>
```

### Technique 3: Argument Analysis
Track how argc/argv are used:
```assembly
# Typical main signature check:
cmp edi, 3          # Check if argc == 3
jne error_path      # Jump to usage message
```

## Step 4: Analyzing Mystery Functions

### Function 1 Analysis (a1b2c3)
```bash
# This function is called with the numeric argument
# Analysis approach:

# 1. Set breakpoint at function start
(gdb) break *<function1_addr>

# 2. Run with different inputs
(gdb) run 5 "test"
(gdb) print $edi    # Input parameter

# 3. Step through and observe
(gdb) si
(gdb) info registers

# 4. Note key operations:
# - Multiplication (imul)
# - Comparison with 100
# - Modulo operation (idiv)
# - Addition of constant
```

### Function 2 Analysis (d4e5f6)
```bash
# This function processes the string argument
# Key observations:

# 1. Memory allocation (call to malloc)
# 2. Loop structure
# 3. Character range checks ('a'-'z', 'A'-'Z')
# 4. Mathematical operations on characters

# Pattern recognition:
# - Adding 13 to character values
# - Modulo 26 operation
# This is ROT13 encoding!
```

### Function 3 Analysis (g7h8i9)
```bash
# This function processes a structure
# Analysis steps:

# 1. Observe parameter passing (RDI points to structure)
# 2. Note structure access patterns:
(gdb) x/10xw $rdi   # Examine structure contents

# 3. Identify operations:
# - Calls function1 with structure field
# - XOR operation with 0xDEADBEEF
# - String copy operation
```

## Step 5: Structure Reconstruction

### Identifying Structure Layout
```assembly
# From disassembly patterns:
mov eax, [rdi]        # Access field at offset 0 (int)
mov [rdi+4], eax      # Access field at offset 4 (int)
lea rax, [rdi+8]      # Access field at offset 8 (char array)
```

### Structure Definition
```c
struct MysteryStruct {
    int field1;      // Offset 0
    int field2;      // Offset 4
    char field3[16]; // Offset 8
};
```

## Step 6: Algorithm Reconstruction

### Function 1 (Number Processor)
```c
int function1(int x) {
    int result = x * x;
    if (result > 100) {
        result = result % 100;
    }
    return result + 42;
}
```

### Function 2 (String Processor)
```c
// ROT13 implementation
char* function2(const char* input) {
    // Allocate memory
    // For each character:
    //   If lowercase: rotate by 13
    //   If uppercase: rotate by 13
    //   Else: keep unchanged
    return output;
}
```

### Function 3 (Structure Processor)
```c
void function3(struct MysteryStruct* ms) {
    ms->field1 = function1(ms->field1);
    ms->field2 = ms->field1 ^ 0xDEADBEEF;
    strcpy(ms->field3, "PROCESSED");
}
```

## Advanced Techniques

### 1. Pattern Matching Script
```python
# GDB Python script to find common patterns
class PatternFinder(gdb.Command):
    def __init__(self):
        super(PatternFinder, self).__init__("find-patterns", gdb.COMMAND_USER)
        
    def invoke(self, arg, from_tty):
        # Find function prologues
        prologue = [0x55, 0x48, 0x89, 0xe5]  # push rbp; mov rbp, rsp
        
        # Search memory
        start = 0x400000
        end = 0x500000
        
        # ... search implementation
```

### 2. Automated Function Boundary Detection
```bash
# Find all potential function starts
objdump -d mystery13 | grep -B1 "push.*rbp" | grep "^[0-9a-f]\+:"
```

### 3. Control Flow Graph Generation
```python
# Using radare2 or Binary Ninja APIs
# Generate visual control flow graphs
# Identify loops, conditionals, and function calls
```

## Validation

### Testing Understanding
1. Predict output for various inputs
2. Verify predictions by running the binary
3. Create a reimplementation and compare behavior

### Example Test Cases
```bash
./mystery13 5 "hello"
# Expected: 
# Processed number: 67 (5*5 + 42)
# Processed string: uryyb (ROT13 of "hello")

./mystery13 12 "ABC"
# Expected:
# Processed number: 86 (144 % 100 + 42)
# Processed string: NOP (ROT13 of "ABC")
```

## Key Takeaways

1. **Systematic Approach**: Start with strings and entry points
2. **Pattern Recognition**: Common compiler patterns help identify constructs
3. **Dynamic Analysis**: Running with different inputs reveals behavior
4. **Cross-Reference**: Use multiple techniques to validate findings
5. **Documentation**: Keep detailed notes during analysis

## Practice Exercises

1. Modify the binary to change the constant 42 to 100
2. Identify and bypass the argc check
3. Find the XOR constant without dynamic analysis
4. Create a script to automatically identify ROT13 functions

## Tools for Stripped Binary Analysis

- **GDB/LLDB**: Dynamic analysis
- **objdump**: Static disassembly
- **strings**: Extract string literals
- **strace/ltrace**: System/library call tracing
- **radare2**: Advanced static analysis
- **IDA Pro/Ghidra**: GUI-based reverse engineering

Remember: Reverse engineering is about pattern recognition, hypothesis testing, and systematic analysis. Practice with increasingly complex binaries to improve your skills.