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

## Lesson: Introduction to x64 Registers and Data Movement

### Understanding x64 Registers
x64 processors have 16 general-purpose registers. Think of registers as the CPU's workspace - extremely fast storage locations where the CPU performs operations.

**Register Hierarchy (using RAX as example):**
```
RAX (64-bit): |--------|--------|--------|--------| (Full 64-bit register)
EAX (32-bit):                   |--------|--------| (Lower 32 bits, zeros upper 32)
 AX (16-bit):                            |--------| (Lower 16 bits)
 AL (8-bit):                             |----|----|  (Lower 8 bits)
 AH (8-bit):                        |----|           (Upper 8 bits of AX)
```

**The 16 General-Purpose Registers:**
- **RAX, RBX, RCX, RDX**: Traditional registers (A=Accumulator, B=Base, C=Counter, D=Data)
- **RSI, RDI**: Source Index, Destination Index (used for string operations)
- **RSP, RBP**: Stack Pointer, Base Pointer (stack management)
- **R8-R15**: Additional registers (introduced in x64)

### The MOV Instruction: Your First Assembly Command
The `mov` instruction copies data from source to destination. It's like assignment in C.

**Syntax:** `mov destination, source`

**Intel vs AT&T Syntax:**
```assembly
# Intel Syntax (easier to read)
mov rax, 42        # Load immediate value 42 into RAX
mov rax, rbx       # Copy RBX contents to RAX
mov rax, [rbp-8]   # Load value from memory address (RBP-8) into RAX

# AT&T Syntax (GCC default)
movq $42, %rax     # Load immediate 42 into RAX  
movq %rbx, %rax    # Copy RBX to RAX (notice reversed order!)
movq -8(%rbp), %rax # Load from memory into RAX
```

**Key Differences:**
- AT&T: `%` prefix for registers, `$` for immediates, source first
- Intel: No prefixes, destination first
- Suffixes: `movq` (quad-word, 64-bit), `movl` (long-word, 32-bit)

### Inline Assembly in C
GCC allows embedding assembly in C code using `__asm__`:

```c
__asm__ volatile (
    "movq %1, %%rax\n\t"        // Load input into RAX
    "movq %%rax, %0\n\t"        // Store RAX to output
    : "=m" (output_variable)     // Output constraint
    : "m" (input_variable)       // Input constraint  
    : "rax"                      // Clobbered registers
);
```

**Constraint Codes:**
- `"=m"`: Output to memory
- `"m"`: Input from memory
- `"%0", "%1"`: Refer to operands by position
- `"%%rax"`: Literal register name (doubled % in inline asm)

### Basic Arithmetic: ADD Instruction
The `add` instruction adds source to destination, storing result in destination.

```assembly
add rax, rbx    # RAX = RAX + RBX
add rax, 10     # RAX = RAX + 10
```

### Step-by-Step Execution
What Tutorial 1 accomplishes:
1. **Load variables from memory into registers**
2. **Perform arithmetic in registers** 
3. **Store result back to memory**

```c
long a = 42;    // C variable in memory
long b = 10;    // C variable in memory
long result;    // C variable for result

// Assembly steps:
// 1. movq a, %rax     - Load a (42) into RAX
// 2. movq b, %rbx     - Load b (10) into RBX  
// 3. addq %rbx, %rax  - Add: RAX = 42 + 10 = 52
// 4. movq %rax, result - Store RAX (52) to result
```

## Problem
Write a program that demonstrates basic register operations and memory access.

### Your Task (Fill in the TODOs)
Now that you understand registers, `mov`, and `add` instructions, complete this program:
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

## Lesson: Comparison Operations and Conditional Execution

### The EFLAGS Register: CPU Status Information
The EFLAGS register contains status bits that are set by arithmetic and comparison operations. Think of it as the CPU's way of remembering what happened in the last operation.

**Key Flags (most important for beginners):**
- **ZF (Zero Flag)**: Set if result was zero
- **SF (Sign Flag)**: Set if result was negative  
- **CF (Carry Flag)**: Set if unsigned overflow occurred
- **OF (Overflow Flag)**: Set if signed overflow occurred

### The CMP Instruction: Making Comparisons
The `cmp` instruction compares two values by subtracting the second from the first, but **only sets flags** - it doesn't store the result.

```assembly
cmp rax, rbx    # Compare RAX with RBX (computes RAX - RBX)
                # Sets flags based on result, but doesn't change RAX or RBX
```

**What happens during comparison:**
```assembly
cmp 15, 25      # Internally computes: 15 - 25 = -10
                # Result is negative, so SF (Sign Flag) = 1
                # Result is not zero, so ZF (Zero Flag) = 0

cmp 25, 25      # Internally computes: 25 - 25 = 0  
                # Result is zero, so ZF (Zero Flag) = 1
```

### Conditional Jumps: Traditional Control Flow
After a comparison, you can use conditional jumps to change program flow:

```assembly
cmp rax, rbx    # Compare RAX and RBX
je equal        # Jump if Equal (ZF = 1)
jg greater      # Jump if Greater (for signed comparison)
jl less         # Jump if Less (for signed comparison)
jmp done        # Unconditional jump

equal:
    # Code for when RAX == RBX
    jmp done

greater:
    # Code for when RAX > RBX  
    jmp done

less:
    # Code for when RAX < RBX

done:
    # Continue execution
```

### Conditional Moves: Modern Efficient Alternative
Conditional moves avoid jumps (which can be slow) by conditionally copying data based on flags:

```assembly
mov rax, 15     # RAX = 15
mov rbx, 25     # RBX = 25
mov rcx, rax    # Assume RAX is the max (RCX = 15)
cmp rax, rbx    # Compare 15 vs 25
cmovl rcx, rbx  # If RAX < RBX (15 < 25), move RBX to RCX
                # Result: RCX = 25 (the larger value)
```

**Common Conditional Move Instructions:**
- `cmove`/`cmovz`: Move if equal/zero (ZF = 1)
- `cmovne`/`cmovnz`: Move if not equal/not zero (ZF = 0)
- `cmovg`/`cmovnle`: Move if greater (signed)
- `cmovl`/`cmovnge`: Move if less (signed)
- `cmovge`/`cmovnl`: Move if greater or equal (signed)
- `cmovle`/`cmovng`: Move if less or equal (signed)

### Step-by-Step: Finding Maximum Value
Here's how Tutorial 2's max function works:

```c
long x = 15;    // First value
long y = 25;    // Second value  
long max_val;   // Result

// Assembly logic:
// 1. Load both values into registers
// 2. Compare them
// 3. Use conditional moves to select the larger one
```

**The Assembly Steps:**
```assembly
movq x, %rax        # Load x (15) into RAX
movq y, %rbx        # Load y (25) into RBX
cmp %rbx, %rax      # Compare: compute RAX - RBX = 15 - 25 = -10
                    # Since result is negative: SF=1, ZF=0
cmovg %rax, %rcx    # If RAX > RBX, move RAX to RCX (doesn't happen)
cmovle %rbx, %rcx   # If RAX <= RBX, move RBX to RCX (this happens!)
                    # Result: RCX = 25
```

### Why Conditional Moves Are Better Than Jumps
1. **No branch prediction**: CPU doesn't have to guess which path to take
2. **Better pipeline performance**: No pipeline flushes from mispredicted jumps
3. **Simpler code**: No need for labels and jump targets

## Problem
Implement a program that performs arithmetic operations and demonstrates flag usage.

### Your Task
Now that you understand comparisons and conditional moves, implement a max function:
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

## Lesson: Implementing Loops with Jumps and Labels

### Understanding Program Flow Control
So far we've learned straight-line execution and conditional moves. Now we need loops - repeating blocks of code. In assembly, loops are built using **labels** and **jumps**.

### Labels: Marking Locations in Code
A label is a name for a specific location in your code. Think of it as a bookmark:

```assembly
start_loop:          # This is a label
    mov rax, 1       # Some instruction
    add rax, 2       # Another instruction
    # ... more code ...
    jmp start_loop   # Jump back to the label
```

**In inline assembly, labels use numbers:**
```assembly
"1:\n\t"             # Label "1"
"movq $1, %%rax\n\t" # Some instruction  
"jmp 1b\n\t"         # Jump backward to label "1"

"2:\n\t"             # Label "2"
"# more code"
"jmp 2f\n\t"         # Jump forward to label "2"
```

**Label syntax:**
- `1:` - Define label "1"
- `1b` - Jump to label "1" **backward** (earlier in code)
- `1f` - Jump to label "1" **forward** (later in code)

### Unconditional Jumps: JMP
The `jmp` instruction always jumps to the specified label:

```assembly
jmp done        # Always jump to label "done"
# This code will never execute
mov rax, 42
done:           # Execution continues here
```

### Conditional Jumps: Making Decisions
Building on Tutorial 2's flags knowledge, conditional jumps let you jump based on previous comparisons:

```assembly
cmp rax, 5      # Compare RAX with 5
je equal        # Jump if equal (RAX == 5)
jg greater      # Jump if greater (RAX > 5)  
jl less         # Jump if less (RAX < 5)
```

**Most useful conditional jumps:**
- `je`/`jz`: Jump if equal/zero (ZF = 1)
- `jne`/`jnz`: Jump if not equal/not zero (ZF = 0)
- `jg`/`jnle`: Jump if greater (signed)
- `jl`/`jnge`: Jump if less (signed)
- `jge`/`jnl`: Jump if greater or equal (signed)
- `jle`/`jng`: Jump if less or equal (signed)

### Building a Basic Loop
Here's the pattern for a counting loop:

```assembly
mov rcx, 5          # Loop counter = 5
1:                  # Start of loop (label 1)
    # ... do something ...
    dec rcx         # Decrement counter (RCX = RCX - 1)
    jnz 1b          # Jump back to label 1 if RCX != 0
```

**Step by step execution:**
1. RCX = 5, do work, RCX becomes 4, 4 ≠ 0 so jump back
2. RCX = 4, do work, RCX becomes 3, 3 ≠ 0 so jump back  
3. RCX = 3, do work, RCX becomes 2, 2 ≠ 0 so jump back
4. RCX = 2, do work, RCX becomes 1, 1 ≠ 0 so jump back
5. RCX = 1, do work, RCX becomes 0, 0 == 0 so DON'T jump (exit loop)

### The DEC Instruction: Decrement by 1
```assembly
dec rax     # RAX = RAX - 1
dec rcx     # RCX = RCX - 1  
```
The `dec` instruction automatically sets flags, so you can immediately use conditional jumps.

### Factorial Algorithm Understanding
Factorial: n! = n × (n-1) × (n-2) × ... × 1

**Examples:**
- 5! = 5 × 4 × 3 × 2 × 1 = 120
- 3! = 3 × 2 × 1 = 6
- 1! = 1
- 0! = 1 (by definition)

**Algorithm in pseudocode:**
```
result = 1
counter = n
while (counter > 1):
    result = result * counter
    counter = counter - 1
return result
```

### The MUL Instruction: Multiplication
The `mul` instruction multiplies RAX by the operand:

```assembly
mov rax, 5      # RAX = 5
mov rbx, 4      # RBX = 4  
mul rbx         # RAX = RAX * RBX = 5 * 4 = 20
                # Result is in RAX (and RDX for overflow, but we ignore that)
```

**Important:** `mul` uses RAX as both source and destination, and may modify RDX.

### Step-by-Step: Factorial Assembly Implementation
```c
// C code: factorial(5)
int n = 5;      
// Expected result: 5! = 120

// Assembly equivalent:
```

```assembly
movq $1, %rax       # result = 1 (RAX holds our result)
movq n, %rbx        # counter = n (RBX is our counter)

1:                  # Start of loop
cmpq $1, %rbx       # Compare counter with 1
jle 2f              # If counter <= 1, exit loop (jump forward to label 2)
mulq %rbx           # result *= counter (RAX = RAX * RBX)  
decq %rbx           # counter-- (RBX = RBX - 1)
jmp 1b              # Jump back to start of loop (label 1 backward)

2:                  # End of loop  
# RAX now contains the factorial result
```

**Execution trace for factorial(5):**
1. result=1, counter=5: 5>1, so result=1×5=5, counter=4
2. result=5, counter=4: 4>1, so result=5×4=20, counter=3  
3. result=20, counter=3: 3>1, so result=20×3=60, counter=2
4. result=60, counter=2: 2>1, so result=60×2=120, counter=1
5. result=120, counter=1: 1≤1, so exit loop
6. Final result: 120

### Common Loop Patterns
**Count-up loop:**
```assembly
xor rax, rax        # RAX = 0 (clear register)
1:
    # do work with RAX
    inc rax         # RAX++
    cmp rax, 10
    jl 1b           # Continue if RAX < 10
```

**Count-down loop (more efficient):**
```assembly
mov rax, 10         # Start at 10
1:
    # do work with RAX  
    dec rax         # RAX--
    jnz 1b          # Continue if RAX != 0
```

## Problem
Implement a factorial function using assembly with loops and jumps.

### Your Task
Now that you understand labels, jumps, and loops, implement factorial:
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

## Lesson: The Stack and x64 Calling Conventions

### Understanding the Stack
The stack is a memory region that grows downward (from high addresses to low addresses). Think of it like a stack of plates - you can only add or remove from the top.

**Stack Operations:**
- **Push**: Add data to top of stack (decreases RSP)
- **Pop**: Remove data from top of stack (increases RSP)

```assembly
push rax        # Push RAX onto stack, RSP = RSP - 8
pop rbx         # Pop top of stack into RBX, RSP = RSP + 8
```

**Visual representation:**
```
Higher Memory Addresses
    ┌─────────────┐
    │   old data  │
    ├─────────────┤ ← RSP after push rax
    │     RAX     │ ← Data pushed
    ├─────────────┤ ← RSP before push
    │   old data  │
    └─────────────┘
Lower Memory Addresses
```

### The Stack Pointer: RSP
RSP (Stack Pointer) always points to the top of the stack. The CPU automatically adjusts RSP during push/pop operations.

### x64 Calling Convention (System V ABI)
When calling functions, we must follow rules about how to pass parameters and preserve registers.

**Parameter Passing (first 6 integer parameters):**
1. **RDI**: First parameter
2. **RSI**: Second parameter  
3. **RDX**: Third parameter
4. **RCX**: Fourth parameter
5. **R8**: Fifth parameter
6. **R9**: Sixth parameter
7. **Stack**: Additional parameters (if more than 6)

**Return Value:**
- **RAX**: Function return value

**Register Preservation:**
- **Caller-saved**: RAX, RCX, RDX, RSI, RDI, R8-R11 (caller must save if needed)
- **Callee-saved**: RBX, RSP, RBP, R12-R15 (function must preserve these)

### Function Prologue and Epilogue
Every function should save and restore the stack frame:

**Standard Prologue:**
```assembly
push rbp        # Save old base pointer
mov rbp, rsp    # Set up new base pointer
# Optional: sub rsp, N  # Allocate local variables
```

**Standard Epilogue:**
```assembly
# Optional: add rsp, N  # Deallocate local variables  
pop rbp         # Restore old base pointer
ret             # Return to caller
```

### The CALL and RET Instructions
**CALL instruction:**
1. Pushes return address (next instruction) onto stack
2. Jumps to the function

**RET instruction:**
1. Pops return address from stack
2. Jumps to that address

```assembly
call my_function    # Push return address, jump to my_function
# ... execution continues here after my_function returns

my_function:
    # function code here
    ret             # Pop return address, jump back
```

### Understanding Recursion
Recursion is when a function calls itself. Each recursive call gets its own stack frame.

**Fibonacci sequence:** F(n) = F(n-1) + F(n-2)
- F(0) = 0
- F(1) = 1  
- F(2) = F(1) + F(0) = 1 + 0 = 1
- F(3) = F(2) + F(1) = 1 + 1 = 2
- F(4) = F(3) + F(2) = 2 + 1 = 3

### Step-by-Step: Recursive Fibonacci
```c
// C equivalent:
long fibonacci(long n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}
```

**Assembly implementation logic:**
1. **Base case check**: If n ≤ 1, return n
2. **Recursive case**: Return fibonacci(n-1) + fibonacci(n-2)
3. **Register management**: Save registers that need preservation

```assembly
fibonacci_asm:
    # Prologue
    push rbp            # Save old base pointer  
    mov rbp, rsp        # Set up stack frame
    push rbx            # Save callee-saved register (we'll use RBX)
    
    # Base case: if n <= 1, return n
    cmp rdi, 1          # Compare n (in RDI) with 1
    jle base_case       # If n <= 1, go to base case
    
    # Recursive case: fibonacci(n-1) + fibonacci(n-2)
    mov rbx, rdi        # Save n in RBX (callee-saved)
    
    # Call fibonacci(n-1)
    dec rdi             # RDI = n - 1
    call fibonacci_asm  # Recursive call
    mov r12, rax        # Save fibonacci(n-1) result
    
    # Call fibonacci(n-2)
    mov rdi, rbx        # Restore n
    sub rdi, 2          # RDI = n - 2
    call fibonacci_asm  # Recursive call
    
    # Add results: fibonacci(n-1) + fibonacci(n-2)
    add rax, r12        # RAX = fibonacci(n-2) + fibonacci(n-1)
    jmp cleanup
    
base_case:
    mov rax, rdi        # Return n (base case: fibonacci(0)=0, fibonacci(1)=1)
    
cleanup:
    # Epilogue
    pop rbx             # Restore callee-saved register
    pop rbp             # Restore base pointer  
    ret                 # Return to caller
```

### Stack Frame Visualization
When fibonacci(3) calls fibonacci(2):

```
Higher Memory
┌─────────────┐
│ Return addr │ ← fibonacci(3)'s stack frame
│  Old RBP    │
│  Saved RBX  │
├─────────────┤ ← Current RSP  
│ Return addr │ ← fibonacci(2)'s stack frame
│  Old RBP    │ 
│  Saved RBX  │
└─────────────┘
Lower Memory
```

### Key Assembly Instructions for Functions
- `call label`: Call function at label
- `ret`: Return from function
- `push reg`: Push register onto stack
- `pop reg`: Pop from stack into register

### Why Preserve Registers?
```assembly
# Bad - destroys caller's data:
my_function:
    mov rbx, 42     # Overwrites caller's RBX!
    ret
    
# Good - preserves caller's data:
my_function:
    push rbx        # Save caller's RBX
    mov rbx, 42     # Use RBX for our work
    # ... do work ...
    pop rbx         # Restore caller's RBX
    ret
```

## Problem
Implement a recursive function in assembly that follows calling conventions.

### Your Task
Now that you understand the stack, calling conventions, and recursion, implement Fibonacci:
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

## Lesson: x64 String Instructions and Memory Operations

### Understanding String Instructions
x64 has specialized instructions for efficient string and memory operations. These instructions can process data byte-by-byte or in larger chunks, and can automatically increment/decrement pointers.

### The Direction Flag (DF)
The Direction Flag controls whether string operations move forward (DF=0) or backward (DF=1) through memory:

```assembly
cld             # Clear Direction Flag (DF=0) - process forward
std             # Set Direction Flag (DF=1) - process backward
```

**Always use `cld` before string operations to ensure forward processing.**

### Core String Instructions

#### SCAS (Scan String)
Compares AL with memory at [RDI], then increments/decrements RDI:

```assembly
mov al, 0       # Look for null terminator (0)
mov rdi, string_ptr
cld             # Process forward
scasb           # Compare AL with [RDI], then RDI++
```

**For string length calculation:**
```assembly
# Find string length
mov rdi, string_ptr
mov al, 0           # Search for null terminator
mov rcx, -1         # Maximum count (very large number)
cld                 # Forward direction
repne scasb         # Repeat while not equal: scan for AL in string
not rcx             # Invert RCX to get positive count
dec rcx             # Subtract 1 (don't count the null terminator)
# RCX now contains string length
```

#### LODS (Load String) and STOS (Store String)
- `lodsb`: Load byte from [RSI] into AL, then increment RSI
- `stosb`: Store AL into [RDI], then increment RDI

```assembly
# Copy one character
lodsb           # AL = [RSI], RSI++
stosb           # [RDI] = AL, RDI++
```

#### String Copy Pattern
```assembly
string_copy_loop:
    lodsb           # Load byte from source [RSI] into AL
    stosb           # Store AL into destination [RDI]
    test al, al     # Check if we copied a null terminator
    jnz string_copy_loop  # If not null, continue copying
```

#### CMPS (Compare Strings)
Compares bytes at [RSI] and [RDI], then increments both:

```assembly
cmpsb           # Compare [RSI] with [RDI], then RSI++, RDI++
```

**For memory comparison:**
```assembly
mov rsi, ptr1
mov rdi, ptr2  
mov rcx, count
cld
repe cmpsb      # Repeat while equal: compare bytes
# If ZF=1 after loop, all bytes were equal
# If ZF=0, found a difference
```

### REP Prefixes: Powerful Loop Automation
REP prefixes automatically repeat string instructions using RCX as counter:

- `rep`: Repeat RCX times
- `repe`/`repz`: Repeat while equal/zero (and RCX > 0)
- `repne`/`repnz`: Repeat while not equal/not zero (and RCX > 0)

```assembly
# Fast memory fill
mov rdi, buffer_ptr
mov al, 0xFF        # Fill with 0xFF bytes
mov rcx, 1000       # Fill 1000 bytes
cld
rep stosb           # Repeat: [RDI] = AL, RDI++, RCX--
```

### Step-by-Step: String Length Implementation
```c
// C equivalent: strlen(str)
size_t strlen(const char* str) {
    const char* s = str;
    while (*s) s++;     // Find null terminator
    return s - str;     // Return difference
}
```

**Assembly approach:**
```assembly
strlen_asm:
    movq string_ptr, %rdi   # Load string address
    xorq %rax, %rax         # Clear RAX (will be our counter)
    movq $-1, %rcx          # Set RCX to maximum value
    cld                     # Clear direction flag
    repne scasb             # Scan for null byte (AL=0 by default)
    notq %rcx               # Invert RCX: -(count+1) becomes count
    decq %rcx               # Don't count the null terminator  
    # RCX now contains string length
```

### Step-by-Step: Memory Comparison
```c
// C equivalent: memcmp(ptr1, ptr2, count)
int memcmp(const void* ptr1, const void* ptr2, size_t count) {
    const unsigned char* p1 = ptr1;
    const unsigned char* p2 = ptr2;
    for (size_t i = 0; i < count; i++) {
        if (p1[i] != p2[i]) {
            return (p1[i] < p2[i]) ? -1 : 1;
        }
    }
    return 0;
}
```

**Assembly approach:**
```assembly
memcmp_asm:
    movq ptr1, %rsi         # Source pointer 1
    movq ptr2, %rdi         # Source pointer 2  
    movq count, %rcx        # Number of bytes to compare
    cld                     # Forward direction
    repe cmpsb              # Compare while equal
    movl $0, %eax           # Default: equal (return 0)
    je equal_exit           # If ZF=1, all bytes matched
    
    # Bytes differed - determine which was larger
    movl $-1, %eax          # Assume ptr1 < ptr2
    jb less_than_exit       # If carry set, ptr1 byte < ptr2 byte
    movl $1, %eax           # Otherwise ptr1 > ptr2
    
less_than_exit:
equal_exit:
    # EAX contains result: -1, 0, or 1
```

### Important String Instruction Notes
1. **Direction Flag**: Always `cld` for forward processing
2. **Register Usage**: 
   - `RSI`: Source pointer (for LODS, CMPS)
   - `RDI`: Destination pointer (for STOS, SCAS, CMPS)
   - `RCX`: Counter (for REP prefixes)
   - `AL`: Data byte (for byte operations)
3. **REP Efficiency**: REP prefixes are highly optimized in hardware

## Problem
Implement efficient string operations using x64 string instructions.

### Skills Covered
- String scanning with `scasb`
- String copying with `lodsb`/`stosb` 
- String comparison with `cmpsb`
- Memory manipulation techniques

### Functions to Implement
Now that you understand string instructions and REP prefixes, implement these functions:
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

## Lesson: Memory Addressing and Array Manipulation

### Understanding Memory Layout
Arrays in memory are stored as contiguous blocks. For an array of `long` values (8 bytes each):

```
Array: [5, 12, 8, 3, 17]
Memory: 
Address: 0x1000  0x1008  0x1010  0x1018  0x1020
Value:     5      12       8       3      17
```

To access element `i`: `address = base_address + (i * element_size)`

### Pointer Arithmetic in Assembly
Unlike C where `ptr++` automatically adjusts for data type size, assembly requires manual size calculation:

```assembly
# For array of longs (8 bytes each):
mov rsi, array_ptr      # RSI points to array[0]
add rsi, 8              # RSI now points to array[1]  
add rsi, 8              # RSI now points to array[2]

# Or use indexing:
mov rax, 2              # Index 2
mov rbx, [rsi + rax*8]  # Load array[2] (base + index * 8)
```

### Memory Addressing Modes
x64 supports flexible addressing: `[base + index*scale + displacement]`
- **base**: Base register (RSI, RDI, etc.)
- **index**: Index register (RAX, RBX, etc.)  
- **scale**: 1, 2, 4, or 8 (matches data type sizes)
- **displacement**: Constant offset

```assembly
mov rax, [rsi + rdx*8 + 16]  # array[index + 2] for 8-byte elements
```

### Efficient Array Traversal Patterns

#### Pattern 1: Counter-Based Loop
```assembly
xor rax, rax            # sum = 0
xor rcx, rcx            # index = 0
1:
    add rax, [rsi + rcx*8]  # sum += array[index]
    inc rcx                 # index++
    cmp rcx, count          # Compare with array size
    jl 1b                   # Continue if index < count
```

#### Pattern 2: Pointer-Based Loop (More Efficient)
```assembly
xor rax, rax            # sum = 0
mov rcx, count          # counter = array size
1:
    add rax, [rsi]      # sum += *pointer
    add rsi, 8          # pointer += sizeof(long)
    dec rcx             # counter--
    jnz 1b              # Continue if counter != 0
```

#### Pattern 3: End-Pointer Loop (Most Efficient)
```assembly
xor rax, rax                # sum = 0
mov rdx, count              # Load count
lea rdx, [rsi + rdx*8]      # end_ptr = array + count*8
1:
    add rax, [rsi]          # sum += *current_ptr
    add rsi, 8              # current_ptr++
    cmp rsi, rdx            # Compare current with end
    jl 1b                   # Continue if current < end
```

### The LEA Instruction: Load Effective Address
`lea` calculates addresses without accessing memory - perfect for pointer arithmetic:

```assembly
lea rax, [rsi + rdx*8 + 16]  # RAX = address of array[index + 2]
# This is faster than:
# mov rax, rdx
# imul rax, 8  
# add rax, rsi
# add rax, 16
```

### Step-by-Step: Array Sum Implementation
```c
// C equivalent:
long array_sum(const long* arr, size_t count) {
    long sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += arr[i];
    }
    return sum;
}
```

**Efficient assembly approach:**
```assembly
array_sum_asm:
    xorq %rax, %rax         # sum = 0 (RAX will hold result)
    movq array_ptr, %rsi    # Load array pointer
    movq count, %rcx        # Load count as loop counter
    testq %rcx, %rcx        # Check if count is zero
    jz done                 # If zero elements, return 0
    
loop_start:
    addq (%rsi), %rax       # sum += *array_ptr
    addq $8, %rsi           # array_ptr += sizeof(long)
    decq %rcx               # count--
    jnz loop_start          # Continue if count != 0
    
done:
    # RAX contains the sum
```

### Step-by-Step: Linear Search Implementation
```c
// C equivalent:
int array_search(const long* arr, size_t count, long target) {
    for (size_t i = 0; i < count; i++) {
        if (arr[i] == target) return i;
    }
    return -1;  // Not found
}
```

**Assembly approach:**
```assembly
array_search_asm:
    movq array_ptr, %rsi    # Array pointer
    movq count, %rcx        # Count
    movq target, %rdx       # Target value
    xorq %rax, %rax         # Index = 0
    
search_loop:
    testq %rcx, %rcx        # Check if any elements left
    jz not_found            # If count is 0, not found
    
    cmpq (%rsi), %rdx       # Compare target with current element
    je found                # If equal, found it!
    
    addq $8, %rsi           # Move to next element
    incq %rax               # Increment index
    decq %rcx               # Decrement counter
    jmp search_loop         # Continue searching
    
found:
    # RAX contains the index
    jmp done
    
not_found:
    movq $-1, %rax          # Return -1 for not found
    
done:
    # RAX contains result: index or -1
```

### Matrix Indexing: 2D Arrays
For a matrix `matrix[rows][cols]`, element `matrix[r][c]` is at:
`address = base + (r * cols + c) * element_size`

```assembly
# matrix[row][col] access for 8-byte elements:
movq row, %rax          # Load row index
mulq cols               # RAX = row * cols  
addq col, %rax          # RAX = row * cols + col
movq (matrix_ptr, %rax, 8), %rbx  # Load matrix[row][col]
```

### Step-by-Step: Matrix Element Access
```c
// C equivalent:
long matrix_get(const long* matrix, size_t rows, size_t cols, 
                size_t row, size_t col) {
    return matrix[row * cols + col];
}
```

**Assembly approach:**
```assembly
matrix_get_asm:
    movq matrix_ptr, %rsi   # Base pointer
    movq row, %rax          # Load row index
    mulq cols               # RAX = row * cols (RDX:RAX = result)
    addq col, %rax          # RAX = row * cols + col (linear index)
    movq (%rsi, %rax, 8), %rax  # Load matrix[linear_index]
    # RAX contains the matrix element
```

### Advanced Technique: Find Maximum
```c
// C equivalent:
long array_max(const long* arr, size_t count) {
    long max = arr[0];
    for (size_t i = 1; i < count; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}
```

**Assembly with conditional moves:**
```assembly
array_max_asm:
    movq array_ptr, %rsi    # Array pointer
    movq count, %rcx        # Count
    movq (%rsi), %rax       # max = arr[0]
    addq $8, %rsi           # Move to arr[1]
    decq %rcx               # count--
    jz done                 # If only one element, done
    
max_loop:
    movq (%rsi), %rdx       # Load current element
    cmpq %rax, %rdx         # Compare with current max
    cmovg %rdx, %rax        # If current > max, update max
    addq $8, %rsi           # Next element
    decq %rcx               # count--
    jnz max_loop            # Continue if more elements
    
done:
    # RAX contains maximum value
```

### Key Addressing Mode Examples
```assembly
mov rax, [rsi]          # rax = *pointer
mov rax, [rsi + 8]      # rax = *(pointer + 1) for 8-byte data
mov rax, [rsi + rcx*8]  # rax = pointer[index] for 8-byte data
mov rax, [rsi + rcx*8 + 16]  # rax = pointer[index + 2]
```

## Problem
Process arrays efficiently using assembly with proper pointer arithmetic.

### Skills Covered
- Array traversal patterns
- Pointer arithmetic in assembly
- Matrix indexing calculations
- Search algorithms

### Functions to Implement
Now that you understand addressing modes and array traversal patterns, implement these functions:
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

## Lesson: Bit-Level Operations and Efficient Algorithms

### Understanding Binary Representation
Every integer is stored as a sequence of bits. Understanding bit patterns is crucial for optimization:

```
42 in binary:  00101010
               ||||||||
Bit position:  76543210
```

### Core Bit Manipulation Instructions

#### Shift Operations
- **SHL** (Shift Left): Multiplies by powers of 2
- **SHR** (Shift Right): Divides by powers of 2 (unsigned)
- **SAR** (Arithmetic Right): Preserves sign bit

```assembly
mov rax, 42     # RAX = 42 (00101010)
shl rax, 1      # RAX = 84 (01010100) - multiply by 2
shr rax, 2      # RAX = 21 (00010101) - divide by 4
```

#### Rotation Operations
- **ROL** (Rotate Left): Bits wrap around
- **ROR** (Rotate Right): Bits wrap around

```assembly
mov rax, 0x8000000000000001  # High and low bits set
rol rax, 1                   # Becomes 0x0000000000000003
```

#### Bitwise Logic Operations
```assembly
and rax, rbx    # RAX = RAX & RBX (bitwise AND)
or  rax, rbx    # RAX = RAX | RBX (bitwise OR)
xor rax, rbx    # RAX = RAX ^ RBX (bitwise XOR)
not rax         # RAX = ~RAX (bitwise NOT)
```

### Bit Manipulation Algorithms

#### Population Count (Count 1 bits)
**Method 1: Loop and test each bit**
```assembly
popcount_loop:
    mov rax, value      # Load value
    xor rbx, rbx        # counter = 0
    
count_loop:
    test rax, rax       # Check if any bits left
    jz done
    mov rcx, rax        # Copy value
    dec rcx             # rcx = value - 1
    and rax, rcx        # Clear lowest set bit: value &= (value-1)
    inc rbx             # count++
    jmp count_loop
done:
    # RBX contains population count
```

**Method 2: Brian Kernighan's algorithm** (more efficient)
This technique `value &= (value - 1)` clears the lowest set bit each iteration.

#### Bit Field Extraction
Extract N bits starting at position P:
```assembly
# Extract bits 8-15 (8 bits starting at position 8)
mov rax, value      # Load value
shr rax, 8          # Shift right by start position
mov rbx, 1          # Create mask
shl rbx, 8          # 2^8 = 256
dec rbx             # 255 = 0xFF (8 bits set)
and rax, rbx        # Apply mask
```

#### Setting/Clearing Specific Bits
```assembly
# Set bit N: value |= (1 << N)
mov rbx, 1
shl rbx, bit_pos    # Create mask with bit N set
or rax, rbx         # Set the bit

# Clear bit N: value &= ~(1 << N)  
mov rbx, 1
shl rbx, bit_pos    # Create mask
not rbx             # Invert mask
and rax, rbx        # Clear the bit
```

#### Power-of-2 Detection
A number is a power of 2 if exactly one bit is set:
```assembly
# Check: (value != 0) && ((value & (value-1)) == 0)
test rax, rax       # Check if value != 0
jz not_power_of_2
mov rbx, rax
dec rbx             # rbx = value - 1
and rax, rbx        # value & (value - 1)
setz al             # Set AL=1 if result is zero
movzx rax, al       # Zero-extend to full register
```

**Why this works:**
- Powers of 2: `1000` (binary)
- Minus 1: `0111` (binary)  
- AND result: `0000` (zero!)
- Non-powers: `1010` & `1001` = `1000` (non-zero)

### Step-by-Step Examples

#### Bit Rotation Implementation
```c
// C equivalent: Rotate left by N positions
uint64_t rotate_left(uint64_t value, int positions) {
    return (value << positions) | (value >> (64 - positions));
}
```

**Assembly approach:**
```assembly
rotate_left_asm:
    mov rax, value      # Load value
    mov cl, positions   # Load shift count (ROL uses CL)
    rol rax, cl         # Rotate left by CL positions
    # RAX contains rotated value
```

#### Efficient Bit Counting
```assembly
# Brian Kernighan's population count
popcount_asm:
    mov rax, value      # Load value to count
    xor rbx, rbx        # Clear counter
    
    test rax, rax       # Check if value is zero
    jz done
    
count_loop:
    mov rcx, rax        # Copy current value
    dec rcx             # rcx = value - 1
    and rax, rcx        # Clear lowest set bit
    inc rbx             # Increment counter
    test rax, rax       # Check if more bits remain
    jnz count_loop      # Continue if non-zero
    
done:
    mov rax, rbx        # Return count in RAX
```

### Advanced Bit Tricks

#### Isolate Lowest Set Bit
```assembly
# Extract lowest set bit: result = value & (-value)
mov rax, value
neg rbx             # rbx = -value (two's complement)
and rax, rbx        # Isolate lowest set bit
```

#### Check if Bit is Set
```assembly
# Test if bit N is set
mov rbx, 1
shl rbx, bit_pos    # Create mask
test rax, rbx       # Test the bit
jnz bit_is_set      # Jump if bit is set
```

#### Count Leading/Trailing Zeros
```assembly
# Count leading zeros (modern CPUs have LZCNT instruction)
lzcnt rax, value    # Count leading zeros

# Count trailing zeros  
bsf rax, value      # Bit scan forward (finds first set bit)
# Result is position of first set bit = number of trailing zeros
```

### Common Bit Manipulation Patterns
1. **Multiply/Divide by powers of 2**: Use SHL/SHR instead of MUL/DIV
2. **Test if even/odd**: `test rax, 1` (check lowest bit)
3. **Clear lowest set bit**: `and rax, (rax-1)`
4. **Set all bits after position N**: `or rax, ((1<<N)-1)`
5. **Toggle bit**: `xor rax, (1<<N)`

## Problem
Implement efficient bit manipulation algorithms using x64 bit operations.

### Skills Covered
- Population count (number of 1 bits)
- Bit field extraction and manipulation
- Bit rotation operations
- Power-of-2 detection

### Functions to Implement
Now that you understand bit manipulation techniques and efficient algorithms, implement these functions:
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

## Lesson: SSE (Streaming SIMD Extensions) Programming

### Understanding SSE and SIMD
SSE allows you to perform operations on multiple data elements simultaneously (SIMD: Single Instruction, Multiple Data). This is extremely powerful for mathematical computations, graphics, and scientific computing.

### SSE Register Architecture
SSE provides 16 128-bit registers: XMM0 through XMM15

```
XMM0: |  float3  |  float2  |  float1  |  float0  |
      127      96 95      64 63      32 31       0
```

Each XMM register can hold:
- **4 single-precision floats** (32-bit each)
- **2 double-precision floats** (64-bit each)
- **Various integer formats**

### Scalar vs Packed Operations

#### Scalar Operations (Single Values)
Process only the lowest element, leaving upper elements unchanged:

```assembly
movss xmm0, [mem]       # Load one float into XMM0[0]
addss xmm0, xmm1        # XMM0[0] = XMM0[0] + XMM1[0]
                        # XMM0[1-3] unchanged
```

#### Packed Operations (Multiple Values)
Process all elements simultaneously:

```assembly
movups xmm0, [mem]      # Load 4 floats into XMM0
addps xmm0, xmm1        # XMM0[i] = XMM0[i] + XMM1[i] for i=0,1,2,3
```

### Core SSE Instructions

#### Data Movement
```assembly
movss xmm0, [mem]       # Load scalar single (32-bit float)
movss [mem], xmm0       # Store scalar single
movups xmm0, [mem]      # Load unaligned packed singles (4 floats)
movaps xmm0, [mem]      # Load aligned packed singles (16-byte aligned)
```

#### Arithmetic Operations
```assembly
# Scalar operations (suffix 'ss' = Scalar Single)
addss xmm0, xmm1        # xmm0[0] += xmm1[0]
subss xmm0, xmm1        # xmm0[0] -= xmm1[0]
mulss xmm0, xmm1        # xmm0[0] *= xmm1[0]
divss xmm0, xmm1        # xmm0[0] /= xmm1[0]
sqrtss xmm0, xmm1       # xmm0[0] = sqrt(xmm1[0])

# Packed operations (suffix 'ps' = Packed Singles)
addps xmm0, xmm1        # xmm0[i] += xmm1[i] for all i
subps xmm0, xmm1        # xmm0[i] -= xmm1[i] for all i
mulps xmm0, xmm1        # xmm0[i] *= xmm1[i] for all i
divps xmm0, xmm1        # xmm0[i] /= xmm1[i] for all i
sqrtps xmm0, xmm1       # xmm0[i] = sqrt(xmm1[i]) for all i
```

### Step-by-Step: Simple SSE Addition
```c
// C equivalent
float add_floats(float a, float b) {
    return a + b;
}
```

**Assembly approach:**
```assembly
add_floats_sse:
    # Arguments: XMM0 = a, XMM1 = b (System V ABI)
    addss xmm0, xmm1        # XMM0[0] = a + b
    # Return value already in XMM0[0]
    ret
```

### Step-by-Step: Vector Addition (4 floats)
```c
// C equivalent: Add two arrays of 4 floats
void add_vectors(float* a, float* b, float* result) {
    for (int i = 0; i < 4; i++) {
        result[i] = a[i] + b[i];
    }
}
```

**Assembly approach (4x faster):**
```assembly
add_vectors_sse:
    # Arguments: RDI = a, RSI = b, RDX = result
    movups xmm0, [rdi]      # Load 4 floats from array a
    movups xmm1, [rsi]      # Load 4 floats from array b
    addps xmm0, xmm1        # Add all 4 pairs simultaneously
    movups [rdx], xmm0      # Store 4 results
    ret
```

**Performance comparison:**
- **Scalar approach**: 4 separate additions
- **SSE approach**: 1 instruction processes 4 values

### Advanced Example: Dot Product
```c
// C equivalent: Compute dot product of two vectors
float dot_product(float* a, float* b, int count) {
    float sum = 0.0f;
    for (int i = 0; i < count; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}
```

**Assembly approach using SSE:**
```assembly
dot_product_sse:
    # Arguments: RDI = a, RSI = b, RDX = count
    xorps xmm0, xmm0        # Clear accumulator (sum = 0.0)
    test rdx, rdx           # Check if count is 0
    jz done
    
    # Process 4 elements at a time
    mov rcx, rdx
    shr rcx, 2              # count / 4 (number of 4-element chunks)
    jz handle_remainder
    
simd_loop:
    movups xmm1, [rdi]      # Load 4 floats from a
    movups xmm2, [rsi]      # Load 4 floats from b
    mulps xmm1, xmm2        # Multiply 4 pairs: xmm1 = a[i] * b[i]
    addps xmm0, xmm1        # Accumulate: sum += products
    
    add rdi, 16             # Advance pointer a (4 floats * 4 bytes)
    add rsi, 16             # Advance pointer b
    dec rcx
    jnz simd_loop
    
    # Horizontal sum: add all 4 elements in xmm0
    haddps xmm0, xmm0       # xmm0 = [a+b, c+d, a+b, c+d]
    haddps xmm0, xmm0       # xmm0 = [sum, sum, sum, sum]
    
handle_remainder:
    # Handle remaining elements (if count not multiple of 4)
    and rdx, 3              # count % 4
    jz done
    
remainder_loop:
    movss xmm1, [rdi]       # Load single float from a
    mulss xmm1, [rsi]       # Multiply with single float from b  
    addss xmm0, xmm1        # Add to sum
    add rdi, 4              # Next element
    add rsi, 4
    dec rdx
    jnz remainder_loop
    
done:
    # Result already in XMM0[0]
    ret
```

### SSE Comparison and Control Flow
```assembly
# Compare packed singles
cmpps xmm0, xmm1, 0     # Compare equal: 0xFFFFFFFF if equal, 0 if not
cmpps xmm0, xmm1, 1     # Compare less than
cmpps xmm0, xmm1, 2     # Compare less or equal
# Result creates mask for conditional operations

# Example: Conditional maximum
maxps xmm0, xmm1        # xmm0[i] = max(xmm0[i], xmm1[i])
minps xmm0, xmm1        # xmm0[i] = min(xmm0[i], xmm1[i])
```

### Data Type Conversions
```assembly
# Float to integer conversion
cvtss2si eax, xmm0      # Convert XMM0[0] to 32-bit integer in EAX
cvtps2pi mm0, xmm0      # Convert 2 floats to 2 integers

# Integer to float conversion  
cvtsi2ss xmm0, eax      # Convert EAX to float in XMM0[0]
```

### Memory Alignment Considerations
SSE works best with 16-byte aligned data:

```assembly
# Aligned loads/stores (faster)
movaps xmm0, [aligned_addr]     # Requires 16-byte alignment
movaps [aligned_addr], xmm0

# Unaligned loads/stores (slower but flexible)
movups xmm0, [unaligned_addr]   # Works with any alignment
movups [unaligned_addr], xmm0
```

### Common SSE Patterns

#### Zero a Register
```assembly
xorps xmm0, xmm0        # Set all bits to 0 (fastest way)
```

#### Broadcast a Value
```assembly
# Broadcast single float to all 4 elements
movss xmm0, [value]     # Load single float
shufps xmm0, xmm0, 0    # Shuffle to replicate across all elements
```

#### Extract/Insert Elements
```assembly
# Extract element 2 (third element) from XMM0 to EAX
extractps eax, xmm0, 2

# Insert EAX into element 1 (second element) of XMM0
insertps xmm0, eax, 0x10
```

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

## Lesson: Integrating Assembly with C++ Code

### Understanding C++ to Assembly Integration
Modern C++ allows seamless integration with assembly code through multiple mechanisms. This tutorial covers how to call assembly functions from C++, how assembly can call back into C++, and how to work with C++ objects in assembly.

### System V ABI and C++ Extensions
The System V ABI defines how C++ extends the basic calling convention:

#### Argument Passing for C++ Objects
```cpp
class SimpleObject {
public:
    int value;
    void method(int arg);
};

// When calling obj.method(42):
// RDI = this pointer (address of obj)
// RSI = arg (42)
```

#### The `this` Pointer
Every non-static C++ method receives an implicit first parameter: the `this` pointer.

```assembly
# C++ method: void MyClass::setValue(int val)
# Signature from assembly perspective: setValue(MyClass* this, int val)
MyClass_setValue:
    # RDI = this pointer
    # RSI = val
    mov [rdi], esi      # this->value = val
    ret
```

### Calling Assembly from C++

#### Method 1: External Assembly Functions
```cpp
// C++ declaration
extern "C" {
    long assembly_function(long a, long b);
    void process_array(int* arr, size_t count);
}

// Usage
int main() {
    long result = assembly_function(10, 20);
    int data[] = {1, 2, 3, 4, 5};
    process_array(data, 5);
}
```

```assembly
# assembly_function.s
.global assembly_function
assembly_function:
    # RDI = a, RSI = b
    add rax, rdi        # result = a
    add rax, rsi        # result += b
    ret

.global process_array
process_array:
    # RDI = arr, RSI = count
    test rsi, rsi
    jz done
    
loop:
    mov eax, [rdi]      # Load array element
    add eax, eax        # Double it
    mov [rdi], eax      # Store back
    add rdi, 4          # Next element
    dec rsi
    jnz loop
done:
    ret
```

#### Method 2: Inline Assembly
```cpp
int inline_assembly_example(int a, int b) {
    int result;
    asm volatile (
        "mov %1, %%eax\n\t"     // Move a to EAX
        "add %2, %%eax\n\t"     // Add b to EAX
        "mov %%eax, %0"         // Store result
        : "=m" (result)         // Output
        : "m" (a), "m" (b)      // Inputs
        : "eax"                 // Clobbered registers
    );
    return result;
}
```

### Assembly Calling C++ Functions

#### Calling C++ Functions from Assembly
```cpp
// C++ function to be called from assembly
extern "C" void cpp_callback(int value) {
    std::cout << "Called from assembly with: " << value << std::endl;
}
```

```assembly
# Assembly code calling C++ function
.extern cpp_callback

call_cpp_function:
    # Prepare to call cpp_callback(42)
    mov rdi, 42         # First argument
    call cpp_callback   # Call C++ function
    ret
```

### Working with C++ Objects in Assembly

#### Simple Object Access
```cpp
struct Point {
    double x, y;
    
    double distance() const;  // Will implement in assembly
};

// C++ usage
Point p = {3.0, 4.0};
double dist = p.distance();
```

```assembly
# Implementation of Point::distance() in assembly
# double Point::distance() - calculate sqrt(x*x + y*y)
.global _ZNK5Point8distanceEv    # Mangled name for Point::distance() const

_ZNK5Point8distanceEv:
    # RDI = this pointer (Point*)
    # Point layout: [x=8bytes][y=8bytes]
    
    movsd xmm0, [rdi]           # Load x into XMM0
    movsd xmm1, [rdi + 8]       # Load y into XMM1
    
    mulsd xmm0, xmm0            # x * x
    mulsd xmm1, xmm1            # y * y
    addsd xmm0, xmm1            # x*x + y*y
    sqrtsd xmm0, xmm0           # sqrt(x*x + y*y)
    
    # Result already in XMM0 (return register for double)
    ret
```

#### Name Mangling Considerations
C++ uses name mangling to support function overloading. You can:

1. **Use `extern "C"`** to disable name mangling:
```cpp
extern "C" double point_distance(const Point* this_ptr);
```

2. **Use `objdump -t` to find mangled names:**
```bash
objdump -t object.o | grep distance
```

3. **Use `nm` command:**
```bash
nm object.o | c++filt  # Demangles names
```

### Advanced: Virtual Function Calls from Assembly

#### Understanding VTable Layout
```cpp
class Base {
public:
    virtual void method1() = 0;
    virtual void method2() = 0;
};

class Derived : public Base {
public:
    void method1() override;
    void method2() override;
};
```

**Memory layout:**
```
Object:  [vtable_ptr][member_data...]
         
VTable:  [method1_addr][method2_addr][...]
```

#### Calling Virtual Functions from Assembly
```assembly
call_virtual_method:
    # RDI = object pointer
    # RSI = method index (0 for method1, 1 for method2)
    
    mov rax, [rdi]              # Load vtable pointer
    mov rax, [rax + rsi*8]      # Load method address (8 bytes per pointer)
    call rax                    # Call virtual method
    ret
```

### Exception Handling Considerations

#### Exception-Safe Assembly
```cpp
class Resource {
public:
    Resource() { /* acquire */ }
    ~Resource() { /* release */ }
};

void safe_function() {
    Resource r;
    assembly_function_that_might_throw();  // Must be exception-safe
}
```

**Guidelines for exception safety:**
1. **Don't throw exceptions from assembly** - use error codes instead
2. **Use RAII wrappers** when calling assembly from C++
3. **Mark assembly functions as `noexcept`** if they don't throw

### Template Integration
```cpp
template<typename T>
T add_template(T a, T b) {
    T result;
    if constexpr (std::is_same_v<T, int>) {
        asm("add %1, %0" : "=r"(result) : "r"(a), "0"(b));
    } else if constexpr (std::is_same_v<T, double>) {
        asm("addsd %1, %0" : "=x"(result) : "x"(a), "0"(b));
    }
    return result;
}
```

### Build Integration

#### Makefile for Mixed Code
```Makefile
CXX = g++
AS = as
CXXFLAGS = -g -O2 -std=c++17

# Mixed executable
mixed_program: main.o assembly_functions.o
	$(CXX) -o $@ $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $<

assembly_functions.o: assembly_functions.s
	$(AS) --64 -o $@ $<
```

#### CMake Integration
```cmake
enable_language(ASM)

add_executable(mixed_program
    main.cpp
    assembly_functions.s
)

target_compile_features(mixed_program PRIVATE cxx_std_17)
```

### Debugging Mixed Code

#### GDB Commands for Mixed Debugging
```bash
# Set breakpoints in both C++ and assembly
(gdb) break main           # C++ function
(gdb) break assembly_func  # Assembly function

# Step between C++ and assembly
(gdb) step                 # Step into assembly from C++
(gdb) finish              # Step out of assembly back to C++

# Examine calling convention
(gdb) info registers      # See argument registers
(gdb) x/i $pc            # Disassemble current instruction
```

### Performance Considerations
1. **Inline assembly** has less call overhead but limits optimization
2. **External assembly functions** allow better optimization but have call overhead
3. **Profile guided optimization** works well with mixed code
4. **Link time optimization** can optimize across C++/assembly boundaries

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

## Lesson: Understanding Compiler Optimizations and Debugging Strategies

### The Challenge of Optimized Code
When you compile with `-O2` or `-O3`, the compiler transforms your code in dramatic ways. Variables disappear, functions get inlined, loops get unrolled, and the assembly bears little resemblance to your source code. This tutorial teaches you to navigate optimized assembly.

### Common Compiler Optimizations

#### 1. Constant Folding and Propagation
**Original code:**
```c
int calculate() {
    int a = 5;
    int b = 10;
    int c = 15;
    return a + b + c;  // 30
}
```

**Unoptimized assembly (-O0):**
```assembly
calculate:
    push rbp
    mov rbp, rsp
    mov DWORD PTR [rbp-4], 5    # a = 5
    mov DWORD PTR [rbp-8], 10   # b = 10  
    mov DWORD PTR [rbp-12], 15  # c = 15
    mov eax, DWORD PTR [rbp-4]  # Load a
    add eax, DWORD PTR [rbp-8]  # Add b
    add eax, DWORD PTR [rbp-12] # Add c
    pop rbp
    ret
```

**Optimized assembly (-O2):**
```assembly
calculate:
    mov eax, 30                 # Compiler calculated 5+10+15=30 at compile time
    ret
```

**What happened:** The compiler calculated the result at compile time since all values were constants.

#### 2. Dead Code Elimination
**Original code:**
```c
int process(int x) {
    int unused = 42;      // Never used
    int y = x * 2;
    if (0) {              // Never true
        return unused;
    }
    return y;
}
```

**Optimized assembly:**
```assembly
process:
    lea eax, [rdi*2]      # eax = x * 2 (using LEA for efficiency)
    ret
```

**What happened:** 
- `unused` variable eliminated
- `if (0)` branch eliminated
- `x * 2` optimized to LEA instruction

#### 3. Loop Unrolling
**Original code:**
```c
void copy_array(int* dest, int* src) {
    for (int i = 0; i < 4; i++) {
        dest[i] = src[i];
    }
}
```

**Unoptimized (with loop):**
```assembly
copy_loop:
    mov eax, DWORD PTR [rsi+rcx*4]  # src[i]
    mov DWORD PTR [rdi+rcx*4], eax  # dest[i] = src[i]
    inc rcx                         # i++
    cmp rcx, 4                      # i < 4?
    jl copy_loop
```

**Optimized (unrolled):**
```assembly
copy_array:
    mov eax, DWORD PTR [rsi]        # dest[0] = src[0]
    mov DWORD PTR [rdi], eax
    mov eax, DWORD PTR [rsi+4]      # dest[1] = src[1]
    mov DWORD PTR [rdi+4], eax
    mov eax, DWORD PTR [rsi+8]      # dest[2] = src[2]
    mov DWORD PTR [rdi+8], eax
    mov eax, DWORD PTR [rsi+12]     # dest[3] = src[3]
    mov DWORD PTR [rdi+12], eax
    ret
```

**What happened:** Loop eliminated, unrolled into sequential operations.

#### 4. Function Inlining
**Original code:**
```c
inline int square(int x) {
    return x * x;
}

int calculate(int a) {
    return square(a) + square(a + 1);
}
```

**Optimized assembly:**
```assembly
calculate:
    lea eax, [rdi+1]        # eax = a + 1
    imul eax, eax           # eax = (a+1)^2
    imul edi, edi           # edi = a^2  
    add eax, edi            # return a^2 + (a+1)^2
    ret
```

**What happened:** Both `square()` calls were inlined, no function call overhead.

#### 5. Register Allocation and Variable Elimination
**Original code:**
```c
int complex_calc(int a, int b, int c) {
    int temp1 = a + b;
    int temp2 = temp1 * c;
    int temp3 = temp2 - a;
    return temp3;
}
```

**Optimized assembly:**
```assembly
complex_calc:
    add edi, esi            # edi = a + b (temp1)
    imul edi, edx           # edi = temp1 * c (temp2)  
    sub edi, [original_a]   # Problem: original 'a' was overwritten!
```

**Better optimized:**
```assembly
complex_calc:
    lea eax, [rdi+rsi]      # eax = a + b
    imul eax, edx           # eax = (a+b) * c
    sub eax, edi            # eax = ((a+b)*c) - a
    ret
```

### Debugging Challenges and Solutions

#### Challenge 1: Missing Variables
**Problem:** Variables don't exist in registers or memory.

**GDB symptoms:**
```bash
(gdb) print temp1
$1 = <optimized out>
(gdb) print temp2  
$2 = <optimized out>
```

**Solutions:**
1. **Use intermediate expressions:**
```bash
(gdb) print $edi + $esi    # If you know temp1 = a + b and a,b in edi,esi
```

2. **Recompile with debug info:**
```bash
gcc -O2 -g3 source.c       # More debug info
```

3. **Use `-Og` optimization:**
```bash
gcc -Og source.c           # Optimize for debugging
```

#### Challenge 2: Inlined Functions
**Problem:** Function calls don't exist in optimized code.

**Debugging approach:**
```bash
# Set breakpoint on calling function
(gdb) break calculate
(gdb) run
(gdb) disassemble           # See the inlined code
(gdb) step                  # Steps through inlined instructions
```

#### Challenge 3: Loop Transformations
**Original loop:**
```c
for (int i = 0; i < n; i++) {
    array[i] = i * 2;
}
```

**Vectorized optimized version:**
```assembly
# Processes 4 elements at once using SIMD
vector_loop:
    movdqu xmm0, [rdi+rax*4]    # Load 4 integers
    pslld xmm0, 1               # Multiply by 2 (shift left)
    movdqu [rsi+rax*4], xmm0    # Store 4 results
    add rax, 4                   # Process next 4 elements
    cmp rax, rcx
    jl vector_loop
```

### Debugging Strategies for Optimized Code

#### 1. Understand the Optimization Level
```bash
# Check optimization level
gcc -Q --help=optimizers | grep enabled
```

#### 2. Compare Optimization Levels
```bash
# Generate assembly at different levels
gcc -O0 -S code.c -o debug.s
gcc -O1 -S code.c -o basic_opt.s  
gcc -O2 -S code.c -o optimized.s
gcc -O3 -S code.c -o aggressive.s

# Compare
diff debug.s optimized.s
```

#### 3. Use Compiler Analysis Tools
```bash
# See what optimizations were applied
gcc -O2 -fverbose-asm -S code.c

# Get optimization report
gcc -O2 -fopt-info-vec-all code.c
```

#### 4. Strategic Breakpoint Placement
Instead of setting breakpoints on missing functions:

```bash
# Set breakpoints on addresses in optimized code
(gdb) break *0x401234

# Set breakpoints on source lines
(gdb) break source.c:42

# Use conditional breakpoints
(gdb) break source.c:42 if variable == target_value
```

#### 5. Reverse Engineering Optimized Assembly

**Pattern recognition for common optimizations:**

```assembly
# Multiplication by constant using LEA
lea eax, [rdi*4]            # eax = rdi * 4
lea eax, [rdi+rdi*2]        # eax = rdi * 3

# Strength reduction (division by constant)
mov eax, 0x55555556         # Magic number for division by 3
imul eax, edi
sar eax, 31                 # edi / 3

# Branch elimination using conditional moves
cmp edi, esi
cmovl eax, edi              # if (a < b) result = a; else result = b;
cmovge eax, esi
```

### Advanced Debugging Techniques

#### Using Hardware Watchpoints
```bash
# Watch memory location even if no variable name
(gdb) watch *0x601040
(gdb) rwatch *0x601040      # Read watchpoint
```

#### Disassembly Analysis
```bash
# Disassemble with source correlation
(gdb) disassemble /s main

# Show instruction boundaries
(gdb) x/20i $pc

# Follow execution flow
(gdb) set disassemble-next-line on
```

#### Compiler-Specific Debugging
```bash
# GCC: Preserve frame pointer for easier debugging
gcc -O2 -fno-omit-frame-pointer code.c

# Disable specific optimizations
gcc -O2 -fno-inline code.c              # Disable inlining
gcc -O2 -fno-unroll-loops code.c        # Disable loop unrolling
```

### Practical Example: Debugging Optimized Fibonacci

**Original code:**
```c
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}
```

**Optimized assembly (with memoization added by compiler):**
```assembly
fibonacci:
    cmp edi, 1
    jle base_case
    
    # Compiler may add memoization or convert to iterative
    # Complex optimized version here...
    
base_case:
    mov eax, edi
    ret
```

**Debugging approach:**
1. **Trace execution path:** Use `stepi` to follow instruction by instruction
2. **Understand register usage:** Track how values flow through registers
3. **Correlate with source:** Use `info source` and `list` to understand context

### Performance Impact of Optimizations
Compare execution characteristics:

```bash
# Time execution
time ./program_O0
time ./program_O2

# Profile with perf
perf record ./program_O2
perf report

# Count instructions
perf stat -e instructions ./program_O2
```

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

## Lesson: C++ Object Memory Layout and Virtual Function Mechanisms

### Understanding C++ Object Layout
C++ objects have a specific memory layout that supports inheritance, virtual functions, and runtime type identification. Understanding this layout is crucial for debugging and optimization.

### Basic Object Layout

#### Simple Class (No Virtual Functions)
```cpp
class Simple {
private:
    int a;
    double b;
    char c;
public:
    void method() { /* non-virtual */ }
};
```

**Memory layout:**
```
Simple object (24 bytes with padding):
┌─────────────┬─────────────┬─────────────┐
│   int a     │  double b   │  char c     │
│  (4 bytes)  │  (8 bytes)  │ (1+7 pad)   │
└─────────────┴─────────────┴─────────────┘
Offset:  0           4           12
```

**Why 24 bytes?** Alignment requires 8-byte boundaries for `double`.

#### Class with Virtual Functions
```cpp
class Virtual {
private:
    int a;
    double b;
public:
    virtual void method1() {}
    virtual void method2() {}
};
```

**Memory layout:**
```
Virtual object (24 bytes):
┌─────────────┬─────────────┬─────────────┐
│  vtable_ptr │   int a     │  double b   │
│  (8 bytes)  │  (4 bytes)  │  (8 bytes)  │ + 4 bytes padding
└─────────────┴─────────────┴─────────────┘
Offset:  0           8           12
```

### Virtual Function Tables (VTables)

#### VTable Structure
Each class with virtual functions has a vtable - a table of function pointers:

```cpp
class Base {
public:
    virtual void func1() { printf("Base::func1\n"); }
    virtual void func2() { printf("Base::func2\n"); }  
    virtual ~Base() {}
};

class Derived : public Base {
public:
    void func1() override { printf("Derived::func1\n"); }
    void func3() { printf("Derived::func3\n"); }
};
```

**VTable layout:**
```
Base's VTable:
┌─────────────┐
│ Base::func1 │ ← vtable[0]
├─────────────┤
│ Base::func2 │ ← vtable[1]  
├─────────────┤
│ Base::~Base │ ← vtable[2]
└─────────────┘

Derived's VTable:
┌─────────────────┐
│ Derived::func1  │ ← vtable[0] (overridden)
├─────────────────┤
│ Base::func2     │ ← vtable[1] (inherited)
├─────────────────┤
│ Derived::~Derived│ ← vtable[2] (overridden)
└─────────────────┘
```

#### Object Creation and VTable Assignment

**C++ code:**
```cpp
Base* obj = new Derived();
```

**Assembly steps:**
```assembly
# Allocate memory
mov rdi, 16                 # Size of Derived object
call malloc                 # Allocate memory
mov rbx, rax                # Save object pointer

# Initialize vtable pointer
mov rax, offset Derived_vtable  # Load vtable address
mov [rbx], rax              # Store vtable pointer in object

# Initialize object members
# ... member initialization code ...

# Return object pointer
mov rax, rbx                # Return new object
```

### Virtual Function Call Mechanism

#### Step-by-Step Virtual Call Process
```cpp
Base* obj = new Derived();
obj->func1();               // Virtual call
```

**Assembly implementation:**
```assembly
# obj->func1() translates to:
mov rax, [rdi]              # 1. Load vtable pointer from object
mov rax, [rax + 0]          # 2. Load func1 address (vtable[0])
mov rdi, rbx                # 3. Set 'this' pointer
call rax                    # 4. Call the function
```

**Detailed breakdown:**
1. **Load vtable pointer**: First 8 bytes of object
2. **Index vtable**: Each virtual function has a fixed index
3. **Load function address**: Dereference vtable[index]
4. **Call function**: With `this` pointer in RDI

#### Comparing Virtual vs Non-Virtual Calls

**Non-virtual call:**
```assembly
# obj->non_virtual_method()
mov rdi, rbx                # Set 'this' pointer
call Base_non_virtual_method  # Direct call - faster
```

**Virtual call:**
```assembly
# obj->virtual_method()
mov rax, [rbx]              # Load vtable pointer
mov rax, [rax + 16]         # Load method address (vtable[2])
mov rdi, rbx                # Set 'this' pointer
call rax                    # Indirect call - slower
```

### Multiple Inheritance Complexity

#### Diamond Inheritance Problem
```cpp
class A {
public:
    virtual void funcA() {}
    int a;
};

class B : virtual public A {
public:
    virtual void funcB() {}
    int b;
};

class C : virtual public A {
public:
    virtual void funcC() {}
    int c;
};

class D : public B, public C {
public:
    virtual void funcD() {}
    int d;
};
```

**Complex object layout for class D:**
```
D object layout (virtual inheritance):
┌─────────────┬─────────────┬─────────────┬─────────────┬─────────────┐
│  B's vtable │   int b     │  C's vtable │   int c     │   int d     │
│  pointer    │             │  pointer    │             │             │
└─────────────┴─────────────┴─────────────┴─────────────┴─────────────┘
      │                           │
      v                           v
┌─────────────┐             ┌─────────────┐
│ B vtable    │             │ C vtable    │
│ + offset to │             │ + offset to │
│   A portion │             │   A portion │
└─────────────┘             └─────────────┘
                                  │
                                  v
                            ┌─────────────┐
                            │ A portion:  │
                            │ A's vtable  │
                            │ int a       │
                            └─────────────┘
```

### Analyzing Objects in GDB

#### Examining Object Layout
```cpp
class TestClass {
public:
    virtual void method1() {}
    virtual void method2() {}
    int member1;
    double member2;
};

TestClass obj;
```

**GDB analysis:**
```bash
# Print object layout
(gdb) print sizeof(TestClass)
$1 = 24

# Examine raw memory
(gdb) x/3x &obj
0x601040:  0x0000000000400abc  0x12345678  0x87654321
#          vtable pointer      member1     member2

# Find vtable
(gdb) x/4x 0x0000000000400abc
0x400abc:  0x0000000000400def  0x0000000000400fed
0x400acc:  0x0000000000400ace  0x0000000000400bce
#          method1 address     method2 address
#          destructor addr     ...

# Disassemble virtual function
(gdb) disassemble 0x0000000000400def
```

#### Tracing Virtual Function Calls
```bash
# Set breakpoint on virtual call
(gdb) break main
(gdb) run
(gdb) stepi                    # Step through instructions

# When you see mov (%rdi), %rax - that's loading vtable
(gdb) print/x $rax             # Show vtable address
(gdb) x/4x $rax               # Show vtable contents
(gdb) stepi                    # Continue to call
```

### Advanced VTable Concepts

#### VTable with Virtual Inheritance
Virtual inheritance creates complex vtables with offset information:

```assembly
# Virtual inheritance vtable entry
vtable_with_virtual_inheritance:
    .quad offset_to_base_A     # Offset to virtual base A
    .quad RTTI_info           # Runtime type information
    .quad Class_method1       # Virtual method 1
    .quad Class_method2       # Virtual method 2
    .quad Class_destructor    # Virtual destructor
```

#### Thunks for Multiple Inheritance
When multiple inheritance requires pointer adjustment:

```assembly
# Thunk function adjusts 'this' pointer before calling real method
Class_B_method_thunk:
    add rdi, 16               # Adjust 'this' pointer offset
    jmp Class_B_method        # Jump to actual method
```

### Runtime Type Information (RTTI)

#### RTTI Structure
```cpp
Base* obj = new Derived();
if (Derived* d = dynamic_cast<Derived*>(obj)) {
    // Successful cast
}
```

**Assembly for dynamic_cast:**
```assembly
# dynamic_cast implementation
mov rax, [rdi]              # Load vtable pointer
mov rax, [rax - 8]          # Load RTTI pointer (before vtable)
mov rsi, offset Derived_RTTI  # Target type RTTI
call __dynamic_cast         # Runtime type checking
```

### Performance Implications

#### Virtual Call Overhead
```assembly
# Direct call (0-1 cycles)
call method_address

# Virtual call (2-3 cycles + possible cache misses)
mov rax, [object]           # Load vtable (1 memory access)
call [rax + offset]         # Call via table (1 memory access)
```

#### Optimization Techniques
Modern compilers use several optimizations:

1. **Devirtualization**: When type is known at compile time
```assembly
# Instead of virtual call
mov rax, [rbx]
call [rax + 16]

# Optimized to direct call
call Derived_method1
```

2. **Speculative devirtualization**: Profile-guided optimization
3. **Virtual table compression**: Shared vtables for similar classes

### Debugging Virtual Function Issues

#### Common Problems and Solutions

**Problem 1: Pure Virtual Function Call**
```bash
# Symptoms in GDB
Program received signal SIGABRT, Aborted.
pure virtual method called
```

**Cause:** Constructor/destructor calling pure virtual function

**Problem 2: Corrupted VTable**
```bash
# Symptoms
(gdb) x/x object_ptr
(gdb) x/x *object_ptr          # Should show vtable
# If vtable pointer is invalid (0x0 or garbage), vtable is corrupted
```

**Investigation steps:**
```bash
# Check object construction
(gdb) break constructor
# Check for buffer overflows affecting vtable pointer
(gdb) watch *(void**)&object   # Watch vtable pointer changes
```

### Practical Example: Analyzing Polymorphism

```cpp
class Shape {
public:
    virtual double area() = 0;
    virtual void draw() = 0;
    virtual ~Shape() {}
protected:
    double x, y;
};

class Circle : public Shape {
    double radius;
public:
    double area() override { return 3.14159 * radius * radius; }
    void draw() override { printf("Drawing circle\n"); }
};

class Rectangle : public Shape {  
    double width, height;
public:
    double area() override { return width * height; }
    void draw() override { printf("Drawing rectangle\n"); }
};
```

**Memory layout analysis:**
```bash
# In GDB
(gdb) print sizeof(Shape)      # Abstract base
(gdb) print sizeof(Circle)     # Concrete derived
(gdb) print sizeof(Rectangle)  # Concrete derived

# Examine vtables
(gdb) info vtbl circle_obj
(gdb) info vtbl rectangle_obj
```

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

## Lesson: Reverse Engineering Without Source Code or Symbols

### The Challenge of Stripped Binaries
When debugging third-party libraries or analyzing security vulnerabilities, you often encounter stripped binaries - executables with no debugging symbols, function names, or source code. This tutorial teaches systematic approaches to understand what the code does.

### Preparation: Creating a Stripped Binary
```bash
# Compile with debug info
gcc -g -o tutorial13_debug tutorial13.c

# Create stripped version (removes symbols)
strip tutorial13_debug -o tutorial13_stripped

# Compare what's lost
nm tutorial13_debug | wc -l      # Shows many symbols
nm tutorial13_stripped | wc -l   # Shows very few symbols
```

### Static Analysis Techniques

#### 1. Basic Information Gathering
```bash
# File type and architecture
file tutorial13_stripped
# Output: ELF 64-bit LSB executable, x86-64

# Entry point and sections
readelf -h tutorial13_stripped
# Shows: Entry point address: 0x401060

# Section analysis
readelf -S tutorial13_stripped
# Shows: .text, .data, .rodata, .bss sections

# String literals (often reveal functionality)
strings tutorial13_stripped
# Look for error messages, format strings, file paths
```

#### 2. Disassembly Analysis
```bash
# Full disassembly
objdump -d tutorial13_stripped > disasm.txt

# Focus on main functions
objdump -d tutorial13_stripped | grep -A 20 "401060:"
```

**Analyzing assembly patterns:**
```assembly
# Look for function prologues
401060: 55          push   %rbp
401061: 48 89 e5    mov    %rsp,%rbp
401064: 48 83 ec 20 sub    $0x20,%rsp

# Look for function epilogues  
4010a0: 48 83 c4 20 add    $0x20,%rsp
4010a4: 5d          pop    %rbp
4010a5: c3          ret
```

#### 3. Control Flow Analysis
**Identifying functions without symbols:**

```assembly
# Function start patterns:
push %rbp           # Stack frame setup
mov %rsp, %rbp      # 
sub $N, %rsp        # Local variables allocation

# Function end patterns:
leave               # Restore stack frame
ret                 # Return to caller

# Or:
add $N, %rsp        # Restore stack
pop %rbp
ret
```

**Finding function boundaries:**
```bash
# Script to find function starts
objdump -d binary | grep -E "push.*%rbp" -A 2 -B 1
```

### Dynamic Analysis Techniques

#### 1. GDB-Based Analysis
```bash
# Start analysis without symbols
gdb tutorial13_stripped

# Set breakpoint at entry point
(gdb) break *0x401060
(gdb) run

# Trace execution flow
(gdb) display/i $pc         # Show current instruction
(gdb) stepi                 # Step one instruction
```

#### 2. Function Discovery Through Execution
```bash
# Find all function calls
(gdb) set logging on
(gdb) set logging file calls.log
(gdb) break *0x401000       # Break near start
(gdb) commands
    silent
    printf "Call to %p\n", $pc
    continue
end
(gdb) run
```

#### 3. Input/Output Correlation
**Strategy:** Modify inputs and observe output changes to understand algorithms.

```bash
# Test with different inputs
echo "test1" | ./tutorial13_stripped
echo "test2" | ./tutorial13_stripped  
echo "AAAA" | ./tutorial13_stripped

# Watch for patterns in output
```

### Algorithm Reconstruction Techniques

#### Example 1: Unknown Mathematical Function
**Assembly code found:**
```assembly
# Function at 0x401120
401120: mov    %edi,%eax        # eax = input (first argument)
401123: imul   %eax,%eax        # eax = input * input  
401126: mov    %eax,%edx        # edx = input^2
401128: mov    %edi,%eax        # eax = input (again)
40112b: add    %edx,%eax        # eax = input + input^2
40112e: mov    $0x7,%ecx        # ecx = 7
401133: cltd                   # Sign extend eax to edx:eax
401134: idiv   %ecx            # eax = (input + input^2) / 7
401136: mov    %edx,%eax       # eax = remainder
401138: ret                    # return remainder
```

**Analysis process:**
1. **Track register usage**: Input in EDI, result in EAX
2. **Identify operations**: Square, add, divide, remainder
3. **Reconstruct algorithm**: `(x + x²) % 7`
4. **Verify with test cases**:
   - Input 3: (3 + 9) % 7 = 12 % 7 = 5
   - Input 5: (5 + 25) % 7 = 30 % 7 = 2

#### Example 2: String Transformation Function
**Assembly pattern analysis:**
```assembly
# String processing loop
401200: mov    (%rsi),%al       # Load character from source
401202: test   %al,%al          # Check for null terminator
401204: je     401230           # Jump to end if null
401206: cmp    $0x61,%al        # Compare with 'a'
401208: jb     401220           # Jump if less than 'a'
40120a: cmp    $0x7a,%al        # Compare with 'z'
40120c: ja     401220           # Jump if greater than 'z'
40120e: add    $0xd,%al         # Add 13 to character
401210: cmp    $0x7a,%al        # Check if > 'z'
401212: jbe    401220           # Jump if <= 'z'
401214: sub    $0x1a,%al        # Subtract 26 (wrap around)
401220: mov    %al,(%rdi)       # Store transformed character
401222: inc    %rsi             # Next source character
401224: inc    %rdi             # Next destination character
401226: jmp    401200           # Loop back
```

**Analysis reveals ROT13 cipher:**
1. Loop through each character
2. If lowercase letter ('a'-'z'): add 13, wrap around if needed
3. Store result
4. Continue to next character

### Pattern Recognition Strategies

#### 1. Common Compiler Patterns
**Loop structures:**
```assembly
# for (i = 0; i < n; i++) pattern
mov $0, %eax          # i = 0
jmp check_condition
loop_start:
    # loop body
    inc %eax          # i++
check_condition:
    cmp %ebx, %eax    # compare i with n
    jl loop_start     # jump if i < n
```

**Function call patterns:**
```assembly
# Standard calling convention
mov $arg3, %rdx       # Third argument
mov $arg2, %rsi       # Second argument  
mov $arg1, %rdi       # First argument
call function_addr    # Call function
```

#### 2. Data Structure Recognition
**Array access patterns:**
```assembly
# arr[i] access
mov (%rdi,%rax,4), %ecx   # Load arr[i] (4-byte elements)
mov (%rdi,%rax,8), %rcx   # Load arr[i] (8-byte elements)
```

**Structure member access:**
```assembly
# struct->member access
mov 8(%rdi), %eax     # Load member at offset 8
mov 16(%rdi), %rdx    # Load member at offset 16
```

### Advanced Analysis Techniques

#### 1. Cross-Reference Analysis
```bash
# Find all references to an address
objdump -d binary | grep "401234"

# Find all calls to a function
objdump -d binary | grep "call.*401234"
```

#### 2. Library Function Identification
**PLT (Procedure Linkage Table) analysis:**
```bash
# List imported functions
objdump -R tutorial13_stripped

# Common library functions have recognizable patterns:
# printf: Format string handling
# malloc: Size argument, returns pointer
# strlen: String scanning loop
```

#### 3. Constant Analysis
**Look for meaningful constants:**
```assembly
mov $0x1f, %eax       # 31 - might be array size
mov $0x400, %ecx      # 1024 - buffer size?
mov $0x10, %edx       # 16 - alignment?
```

### Systematic Reverse Engineering Workflow

#### Phase 1: Initial Reconnaissance
```bash
# 1. Basic file analysis
file target_binary
strings target_binary | head -20
readelf -h target_binary

# 2. Identify entry point and main functions
objdump -d target_binary | grep -E "(main|_start)"
```

#### Phase 2: Static Analysis
```bash
# 3. Full disassembly with annotations
objdump -d target_binary > full_disasm.txt

# 4. Find function boundaries
grep -E "push.*%rbp" full_disasm.txt

# 5. Analyze control flow
# Look for jumps, calls, returns
grep -E "(jmp|call|ret)" full_disasm.txt
```

#### Phase 3: Dynamic Analysis
```bash
# 6. GDB session for function discovery
gdb target_binary
(gdb) set disassemble-next-line on
(gdb) break *entry_point
(gdb) run
(gdb) continue    # Let it run and observe

# 7. Trace key functions
(gdb) break *interesting_address
(gdb) commands
    silent
    printf "Function called with args: %d, %d\n", $edi, $esi
    continue
end
```

#### Phase 4: Algorithm Reconstruction
```bash
# 8. Test hypothesis with different inputs
echo "input1" | ./target_binary
echo "input2" | ./target_binary

# 9. Verify understanding by reimplementing
# Write C code that should produce same output
```

### Practical Example: Analyzing Unknown Binary

**Given:** A binary that processes numbers and outputs results

**Step 1: Static analysis reveals:**
```assembly
# Main function calls function at 0x401150
call 401150

# Function 0x401150:
401150: mov %edi, %eax        # Copy first argument
401153: imul %eax, %eax       # Square it
401156: mov $100, %ecx        # Load constant 100
40115b: cltd                  # Sign extend
40115c: idiv %ecx             # Divide by 100
40115e: mov %edx, %eax        # Return remainder
401160: ret
```

**Step 2: Dynamic testing:**
```bash
echo "7" | ./mystery_binary    # Output: 49
echo "15" | ./mystery_binary   # Output: 25
echo "23" | ./mystery_binary   # Output: 29
```

**Step 3: Algorithm reconstruction:**
- Function computes `(input * input) % 100`
- Returns last two digits of the square

**Step 4: Verification:**
- 7² = 49, 49 % 100 = 49 ✓
- 15² = 225, 225 % 100 = 25 ✓  
- 23² = 529, 529 % 100 = 29 ✓

### Tools and Automation

#### Custom Analysis Scripts
```python
# Python script to find function patterns
import re

with open('disasm.txt', 'r') as f:
    content = f.read()

# Find all function prologues
prologues = re.findall(r'(\w+):\s+55\s+push\s+%rbp', content)
print("Function starts:", prologues)
```

#### GDB Scripting
```bash
# .gdbinit script for automated analysis
define analyze_function
    set $func_start = $arg0
    set $pc = $func_start
    printf "Analyzing function at %p\n", $func_start
    
    # Step through and log key instructions
    while $pc < $func_start + 0x100
        stepi
        if ($pc & 0xf) == 0
            printf "  %p: ", $pc
            x/i $pc
        end
    end
end
```

### Common Reverse Engineering Challenges

#### Challenge 1: Obfuscated Code
**Anti-analysis techniques:**
- Dead code insertion
- Control flow obfuscation
- String encryption

**Solutions:**
- Focus on data flow rather than control flow
- Use dynamic analysis to bypass obfuscation
- Identify key algorithms despite obfuscation

#### Challenge 2: Complex Data Structures
**Approach:**
- Look for consistent offset patterns
- Identify structure size through allocation calls
- Use memory watchpoints to understand access patterns

#### Challenge 3: Cryptographic Functions
**Recognition patterns:**
- Large constant tables (S-boxes)
- Bit manipulation operations
- Fixed iteration counts
- Mathematical operations on large numbers

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

## Lesson: Debugging Runtime-Loaded Shared Libraries

### Understanding Dynamic Library Loading
Dynamic libraries (.so files on Linux, .dylib on macOS, .dll on Windows) can be loaded in two ways:
1. **Load-time linking**: Specified with `-l` at compile time
2. **Runtime loading**: Loaded with `dlopen()` during execution

This tutorial focuses on runtime loading, which is more complex to debug because the library isn't available when the program starts.

### Dynamic Loading Lifecycle

#### The dlopen() Family of Functions
```c
#include <dlfcn.h>

// Load library into memory
void* dlopen(const char* filename, int flags);

// Get address of symbol from library  
void* dlsym(void* handle, const char* symbol);

// Get error information
char* dlerror(void);

// Unload library from memory
int dlclose(void* handle);
```

**Flags for dlopen():**
- `RTLD_LAZY`: Lazy symbol resolution (resolve when needed)
- `RTLD_NOW`: Resolve all symbols immediately
- `RTLD_GLOBAL`: Make symbols available to subsequently loaded libraries
- `RTLD_LOCAL`: Keep symbols private to this library

#### Step-by-Step Loading Process
```c
// Example loading sequence
void* handle = dlopen("./libshared.so", RTLD_LAZY);
if (!handle) {
    fprintf(stderr, "Error: %s\n", dlerror());
    return;
}

// Get function pointer
int (*lib_func)(int) = dlsym(handle, "my_function");
if (!lib_func) {
    fprintf(stderr, "Symbol error: %s\n", dlerror());
    dlclose(handle);
    return;
}

// Use the function
int result = lib_func(42);

// Clean up
dlclose(handle);
```

### Memory Layout During Dynamic Loading

#### Before Library Load
```
Program Memory Layout:
┌─────────────────┐ 0x7fffffffffff
│     Stack       │
├─────────────────┤
│       ...       │
├─────────────────┤ 0x7ffff7a00000
│   libc.so.6     │ (already loaded)
├─────────────────┤ 0x7ffff7800000  
│   ld-linux.so   │ (dynamic linker)
├─────────────────┤
│       ...       │
├─────────────────┤ 0x400000
│  Main program   │
└─────────────────┘
```

#### After dlopen()
```
Program Memory Layout:
┌─────────────────┐
│     Stack       │
├─────────────────┤
│       ...       │
├─────────────────┤ 0x7ffff7c00000
│  libshared.so   │ ← Newly loaded library
├─────────────────┤ 0x7ffff7a00000
│   libc.so.6     │
├─────────────────┤ 0x7ffff7800000
│   ld-linux.so   │
├─────────────────┤
│       ...       │
├─────────────────┤ 0x400000
│  Main program   │
└─────────────────┘
```

### GDB Techniques for Dynamic Library Debugging

#### 1. Catching Library Load Events
```bash
# Set up to catch library loading
(gdb) catch load
(gdb) catch load libshared.so    # Specific library
(gdb) run

# When library loads:
# Stopped due to shared library event:
#   Inferior loaded /path/to/libshared.so

# Check what libraries are loaded
(gdb) info sharedlibrary
# Shows: libshared.so loaded at 0x7ffff7bd1000
```

#### 2. Breaking on Dynamic Loading Functions
```bash
# Break on the dlopen call
(gdb) break dlopen
(gdb) run

# At dlopen breakpoint:
(gdb) print (char*)$rdi          # Print library name
(gdb) print $rsi                 # Print flags
(gdb) continue

# Break on dlsym 
(gdb) break dlsym
# When hit:
(gdb) print (char*)$rsi          # Print symbol name being looked up
```

#### 3. Setting Breakpoints Before Library is Loaded
The challenge: You want to debug a function that doesn't exist yet.

**Solution: Pending breakpoints**
```bash
# This will fail initially:
(gdb) break lib_process_data
Function "lib_process_data" not defined.
Make breakpoint pending on future shared library load? (y or [n]) y

# Breakpoint is now pending until library loads
(gdb) info breakpoints
Num     Type           Disp Enb Address    What
1       breakpoint     keep y   <PENDING>  lib_process_data

# After library loads, GDB resolves the breakpoint automatically
```

#### 4. Manual Symbol Resolution
If symbols aren't available, find functions manually:

```bash
# Find library base address
(gdb) info proc mappings | grep libshared
0x7ffff7bd1000     0x7ffff7bd2000        0x1000        0x0 /path/to/libshared.so

# Look at exported symbols
(gdb) shell nm -D /path/to/libshared.so
0000000000001169 T lib_process_data

# Calculate actual address: base + offset
# 0x7ffff7bd1000 + 0x1169 = 0x7ffff7bd2169

# Set breakpoint at calculated address
(gdb) break *0x7ffff7bd2169
```

### Advanced Debugging Scenarios

#### Scenario 1: Library Constructor/Destructor Debugging
Libraries can have initialization and cleanup code that runs automatically:

```c
// In library source:
__attribute__((constructor))
void lib_init(void) {
    printf("Library initialized\n");
    // Initialization code
}

__attribute__((destructor))  
void lib_cleanup(void) {
    printf("Library cleanup\n");
    // Cleanup code
}
```

**Debugging constructors:**
```bash
# Set breakpoint on constructor (if symbol available)
(gdb) break lib_init

# Or catch the dlopen that triggers constructors
(gdb) catch load libshared.so
(gdb) run
# When library loads, constructors run automatically
```

#### Scenario 2: Callback Debugging
Libraries often call back into the main program:

```c
// Main program provides callback
int my_callback(int value) {
    return value * 2;
}

// Library calls the callback
void lib_execute_callback(int (*callback)(int), int data) {
    int result = callback(data);  // Calls back into main program
    printf("Callback returned: %d\n", result);
}
```

**Debugging callbacks:**
```bash
# Set breakpoint in the callback function
(gdb) break my_callback
(gdb) run

# When hit, examine the call stack
(gdb) backtrace
#0  my_callback (value=42) at main.c:15
#1  0x7ffff7bd2234 in lib_execute_callback () from ./libshared.so
#2  0x0000000000401234 in main () at main.c:30

# You can see the call path: main → library → callback
```

#### Scenario 3: Debugging Without Any Symbols
When the library is completely stripped:

```bash
# Find library boundaries
(gdb) info proc mappings | grep libshared
0x7ffff7bd1000     0x7ffff7bd2000     0x1000     0x0  /path/to/libshared.so

# Disassemble from the beginning
(gdb) x/100i 0x7ffff7bd1000

# Look for function prologues
(gdb) find /h 0x7ffff7bd1000, 0x7ffff7bd2000, 0x4855
# Searches for "push %rbp" instruction (0x55) followed by "mov %rsp,%rbp" (0x48)

# Set breakpoints on discovered functions
(gdb) break *0x7ffff7bd1123
```

### Library Dependency Analysis

#### Understanding PLT and GOT
**PLT (Procedure Linkage Table)**: Stubs for calling external functions
**GOT (Global Offset Table)**: Table of addresses for external symbols

```bash
# Examine PLT entries
(gdb) info symbol 0x401030
dlopen@plt in section .plt of /path/to/program

# Show GOT entries  
(gdb) x/10x 0x404000    # Typical GOT location
```

#### Tracing Symbol Resolution
```bash
# Enable loader debugging (outside GDB)
LD_DEBUG=symbols ./program 2>symbols.log

# Or within GDB:
(gdb) set environment LD_DEBUG=symbols
(gdb) run 2>symbols.log

# The log shows symbol binding:
# symbol=lib_process_data;  lookup in file=./libshared.so [0]
# binding file ./libshared.so [0] to ./program [0]: normal symbol 'lib_process_data'
```

### Memory Management Debugging

#### Detecting Library Unload Issues
```bash
# Set breakpoint on dlclose
(gdb) break dlclose
(gdb) run

# After dlclose, the library memory may be unmapped
# Accessing it will cause segfault
(gdb) continue
# Program received signal SIGSEGV, Segmentation fault.
# 0x7ffff7bd1234 in ?? ()

# Check if address is still mapped
(gdb) info proc mappings | grep 7ffff7bd1234
# Should show nothing if unmapped
```

#### Watching Library Global Variables
```bash
# Get address of global variable in library
(gdb) print &lib_global_counter
$1 = (int *) 0x7ffff7bd3008

# Set watchpoint on the variable
(gdb) watch *0x7ffff7bd3008
(gdb) run

# Execution stops whenever the variable changes
# Hardware watchpoint 1: *0x7ffff7bd3008
# Old value = 0
# New value = 1
```

### Platform-Specific Debugging

#### Linux-Specific Tools
```bash
# Show all loaded libraries
ldd ./program

# Trace library calls
ltrace -l ./libshared.so ./program

# Trace system calls related to loading
strace -e openat ./program 2>&1 | grep "\.so"

# Library search path debugging
LD_DEBUG=libs ./program
```

#### Environment Variables for Debugging
```bash
# Show library search process
LD_DEBUG=libs ./program

# Show symbol binding
LD_DEBUG=bindings ./program  

# Show all debug info
LD_DEBUG=all ./program

# Preload a library (useful for interception)
LD_PRELOAD=./debug_lib.so ./program
```

### Common Dynamic Library Issues

#### Issue 1: Symbol Not Found
**Problem:**
```
./program: symbol lookup error: ./libshared.so: undefined symbol: missing_function
```

**Debugging:**
```bash
# Check what symbols the library expects
nm -u libshared.so | grep missing_function

# Check what symbols are provided
nm -D program | grep missing_function

# The symbol might be in another library that needs to be linked
```

#### Issue 2: Version Conflicts
**Problem:** Multiple versions of the same library loaded

**Debugging:**
```bash
# Check library versions
(gdb) info sharedlibrary
# Shows multiple versions:
# 0x7ffff7a00000  libfoo.so.1
# 0x7ffff7c00000  libfoo.so.2

# Use library versioning
objdump -p libshared.so | grep SONAME
```

#### Issue 3: Library Not Found
**Problem:**
```
dlopen: libshared.so: cannot open shared object file: No such file or directory
```

**Debugging:**
```bash
# Check current directory and LD_LIBRARY_PATH
(gdb) shell ls -la libshared.so
(gdb) show environment LD_LIBRARY_PATH

# Set library path in GDB
(gdb) set environment LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH
```

### Automation and Scripting

#### GDB Script for Library Analysis
```python
# library_debug.py - GDB Python script
import gdb

class LibraryLoader(gdb.Command):
    def __init__(self):
        super(LibraryLoader, self).__init__("libload", gdb.COMMAND_USER)
    
    def invoke(self, arg, from_tty):
        # Set up library loading breakpoints
        gdb.execute("catch load")
        gdb.execute("break dlopen")
        gdb.execute("break dlsym")
        
        print("Library debugging enabled")
        print("Breakpoints set on: load events, dlopen, dlsym")

LibraryLoader()
```

**Usage:**
```bash
(gdb) source library_debug.py
(gdb) libload
(gdb) run
```

#### Automated Symbol Discovery
```bash
# Script to find and break on all exported functions
for func in $(nm -D libshared.so | grep ' T ' | cut -d' ' -f3); do
    echo "break $func"
done > breakpoints.gdb

# Load in GDB
(gdb) source breakpoints.gdb
```

### Performance Considerations

#### Library Loading Overhead
```bash
# Measure loading time
time ./program

# Profile with detailed timing
perf record -g ./program
perf report

# Count library loading events
strace -c ./program
```

#### Symbol Resolution Performance
```bash
# Compare lazy vs immediate binding
# Lazy (default):
LD_BIND_NOW=0 time ./program

# Immediate:  
LD_BIND_NOW=1 time ./program
```

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