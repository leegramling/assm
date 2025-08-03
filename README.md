# x64 Assembly and GDB Learning Suite

A comprehensive tutorial series for learning x64 assembly language and GDB debugging, designed for developers who need to debug C++ libraries at the assembly level.

## Overview

This repository contains a progressive learning path that takes you from basic x64 assembly concepts to advanced debugging techniques. Perfect for developers who need to work with stripped binaries or debug third-party libraries where only assembly code is visible.

## Files and Structure

### Core Documentation
- **`tut.md`** - Complete tutorial guide with x64 assembly fundamentals, GDB commands, and IDE setup
- **`CLAUDE.md`** - Development workflow and project guidance for Claude Code
- **`todo.md`** - Task tracking with priority-based organization
- **`README.md`** - This file

### Tutorial Series

#### Tutorial 1: Basic Registers and Memory Operations
- **Files**: `tutorial1.c`, `tutorial1_complete.c`
- **Focus**: Basic register usage, mov instructions, inline assembly syntax
- **Skills**: Loading values, basic arithmetic, understanding AT&T vs Intel syntax

#### Tutorial 2: Arithmetic Operations and Flags
- **Files**: `tutorial2.c`, `tutorial2_complete.c`  
- **Focus**: Comparison operations, conditional moves, flag usage
- **Skills**: Using cmp instruction, conditional execution without jumps

#### Tutorial 3: Control Flow and Loops
- **Files**: `tutorial3.c`, `tutorial3_complete.c`
- **Focus**: Implementing loops with assembly, conditional jumps
- **Skills**: Labels, jump instructions, factorial implementation

#### Tutorial 4: Function Calls and Stack Management
- **Files**: `tutorial4.c`, `tutorial4_complete.c`
- **Focus**: x64 calling conventions, recursive functions, stack frames
- **Skills**: Proper register preservation, System V ABI compliance

#### Tutorial 5: Debugging C++ Libraries with GDB
- **Files**: `library.cpp`, `main.cpp`, `debug_tutorial.md`
- **Focus**: Assembly-level debugging of stripped binaries
- **Skills**: Reverse engineering, GDB assembly debugging, function analysis

## Quick Start

### Prerequisites
- GCC or Clang compiler
- GDB debugger
- VSCode or CLion (optional, for IDE debugging)

### Running the Tutorials

1. **Start with the fundamentals**:
   ```bash
   # Read the main tutorial guide
   cat tut.md
   ```

2. **Work through each tutorial**:
   ```bash
   # Tutorial 1: Basic operations
   gcc -g -o tutorial1 tutorial1_complete.c && ./tutorial1
   
   # Tutorial 2: Conditionals
   gcc -g -o tutorial2 tutorial2_complete.c && ./tutorial2
   
   # Tutorial 3: Loops
   gcc -g -o tutorial3 tutorial3_complete.c && ./tutorial3
   
   # Tutorial 4: Function calls
   gcc -g -o tutorial4 tutorial4_complete.c && ./tutorial4
   ```

3. **Practice debugging**:
   ```bash
   # Tutorial 5: Debug a "black box" library
   g++ -c -O0 library.cpp -o library.o
   g++ -g -O0 main.cpp library.o -o debug_example
   gdb ./debug_example
   ```

### Learning Path

1. **Read `tut.md`** - Essential background on x64 assembly and GDB
2. **Try each tutorial** - Start with the incomplete `.c` files, then check your work against `_complete.c` versions
3. **Practice debugging** - Use Tutorial 5 to simulate real-world debugging scenarios
4. **Set up your IDE** - Configure VSCode or CLion for assembly debugging

## Key Concepts Covered

### x64 Assembly Fundamentals
- Register usage and naming conventions
- AT&T vs Intel syntax differences  
- x64 calling conventions (System V ABI)
- Memory addressing modes
- Common instruction patterns

### GDB Debugging Techniques
- Setting Intel syntax for readability
- Assembly-level breakpoints and stepping
- Register and memory examination
- Debugging stripped binaries
- Reverse engineering workflows

### IDE Integration
- VSCode debugging configuration
- CLion assembly debugging setup
- GDB integration and automation

## Expected Outputs

Each tutorial produces specific, verifiable output:

- **Tutorial 1**: `Result: 52`
- **Tutorial 2**: `Max of 15 and 25 is: 25`
- **Tutorial 3**: `Factorial of 5 is: 120`
- **Tutorial 4**: Fibonacci sequence 0-10
- **Tutorial 5**: `Result: 35` (after reverse engineering)

## Real-World Applications

This tutorial series prepares you for:
- Debugging optimized or stripped C++ libraries
- Performance analysis at the assembly level
- Security analysis and reverse engineering
- Understanding compiler optimizations
- Low-level system programming

## Next Steps

After completing these tutorials:
1. Practice with real C++ libraries
2. Learn SIMD instructions (SSE, AVX)
3. Understand different optimization levels
4. Explore other calling conventions
5. Study compiler-generated assembly patterns

## File Organization

```
/
├── README.md              # This overview
├── tut.md                 # Complete tutorial guide
├── CLAUDE.md              # Development workflow
├── todo.md                # Task tracking
├── tutorial1.c            # Basic registers (skeleton)
├── tutorial1_complete.c   # Basic registers (solution)
├── tutorial2.c            # Arithmetic/flags (skeleton)
├── tutorial2_complete.c   # Arithmetic/flags (solution)
├── tutorial3.c            # Control flow (skeleton)
├── tutorial3_complete.c   # Control flow (solution)
├── tutorial4.c            # Function calls (skeleton)
├── tutorial4_complete.c   # Function calls (solution)
├── library.cpp            # Debug target library
├── main.cpp               # Debug main program
└── debug_tutorial.md      # Tutorial 5 instructions
```

Start with `tut.md` for the complete learning experience!# assm
