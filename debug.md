# Debugging C++ Libraries Without Symbols

A comprehensive guide for debugging stripped C++ libraries at the assembly level when no debug symbols are available.

## Table of Contents
1. [Initial Assessment](#initial-assessment)
2. [Function Identification](#function-identification)
3. [C++ Name Mangling](#c-name-mangling)
4. [Object and VTable Analysis](#object-and-vtable-analysis)
5. [Common C++ Patterns](#common-c-patterns)
6. [Debugging Strategies](#debugging-strategies)
7. [Advanced Techniques](#advanced-techniques)
8. [Case Studies](#case-studies)

## Initial Assessment

### 1. Gather Basic Information

```bash
# Check file type and architecture
file libexample.so
# Output: ELF 64-bit LSB shared object, x86-64

# Check for symbols (even minimal ones)
nm -D libexample.so | head -20

# List dependencies
ldd libexample.so

# Check sections
objdump -h libexample.so

# Look for strings
strings libexample.so | less
```

### 2. Identify Entry Points

```bash
# Find exported functions
objdump -T libexample.so | grep -E 'DF .text'

# Find PLT entries (external calls)
objdump -d libexample.so | grep '@plt'

# Common entry points to look for:
# - _init, _fini
# - Global constructors/destructors
# - Exported API functions
```

## Function Identification

### Finding Function Boundaries

```assembly
# Function prologue patterns
55                      push   %rbp
48 89 e5                mov    %rsp,%rbp
48 83 ec XX             sub    $XX,%rsp

# Function epilogue patterns
c9                      leave  
c3                      ret
5d                      pop    %rbp
c3                      ret

# Alternative prologue (optimized)
48 83 ec XX             sub    $XX,%rsp
```

### GDB Script for Function Discovery

```bash
# Save as find_functions.gdb
set pagination off
set logging on

# Scan for function prologues
set $addr = 0x1000
while $addr < 0x50000
    set $inst = *(unsigned char *)$addr
    if $inst == 0x55
        set $next = *(unsigned char *)($addr+1)
        if $next == 0x48
            printf "Potential function at: 0x%x\n", $addr
        end
    end
    set $addr = $addr + 1
end
```

## C++ Name Mangling

### Understanding Mangled Names

```bash
# Common patterns:
_Z                      # Prefix for C++ names
N...E                   # Nested names
St                      # std namespace
SS                      # std::string
i                       # int
PKc                     # const char*
v                       # void

# Examples:
_ZN7MyClass10myFunctionEii
# MyClass::myFunction(int, int)

_ZNSt6vectorIiEC1Ev
# std::vector<int>::vector()
```

### Demangling Techniques

```bash
# Using c++filt
echo "_ZN7MyClass10myFunctionEii" | c++filt

# In GDB
(gdb) set print demangle on
(gdb) info symbol 0x401234

# Manual identification
# Look for patterns in strings output
strings libexample.so | grep -E '^_Z' | c++filt
```

## Object and VTable Analysis

### Identifying C++ Objects

```assembly
# VTable pointer usually at offset 0
mov    (%rdi),%rax      # Load vtable pointer
mov    0x8(%rax),%rax   # Load function pointer from vtable
callq  *%rax            # Virtual function call

# Constructor pattern
lea    0x201234(%rip),%rax  # Load vtable address
mov    %rax,(%rdi)           # Store at object start

# Member access pattern
mov    0x8(%rdi),%eax   # Access member at offset 8
```

### VTable Structure Analysis

```bash
# In GDB, examine potential vtables
(gdb) x/10xg 0x601230   # Examine vtable contents

# Look for:
# - Sequence of function pointers
# - NULL terminators
# - RTTI information (usually before vtable)
```

## Common C++ Patterns

### 1. String Operations

```assembly
# std::string construction
lea    -0x20(%rbp),%rdi
call   _ZNSsC1Ev        # string constructor

# std::string assignment
lea    -0x20(%rbp),%rdi
mov    %rax,%rsi
call   _ZNSsaSERKSs     # string::operator=
```

### 2. Vector Operations

```assembly
# std::vector push_back
lea    -0x30(%rbp),%rdi
lea    -0x4(%rbp),%rsi
call   _ZNSt6vectorIiE9push_backERKi

# Vector access pattern
mov    (%rdi),%rax      # vector._M_start
mov    0x8(%rdi),%rdx   # vector._M_finish
```

### 3. Exception Handling

```assembly
# Try block entry
call   __cxa_begin_catch

# Throw pattern
mov    $0x8,%edi
call   __cxa_allocate_exception
```

### 4. Smart Pointers

```assembly
# shared_ptr operations
call   _ZNSt12__shared_ptrIiLN9__gnu_cxx12_Lock_policyE2EEC1IivEEPT_

# unique_ptr reset
call   _ZNSt10unique_ptrIiSt14default_deleteIiEE5resetEPi
```

## Debugging Strategies

### 1. Dynamic Analysis Workflow

```bash
# Step 1: Set catchpoints for library load
(gdb) catch load libexample.so

# Step 2: Set breakpoints on PLT entries
(gdb) break malloc@plt
(gdb) break _Znwm@plt  # operator new

# Step 3: Trace execution
(gdb) record full
(gdb) continue
(gdb) record instruction-history
```

### 2. Memory Pattern Analysis

```bash
# Watch for object allocation patterns
(gdb) break malloc
(gdb) commands
  silent
  if $rdi == 24
    printf "Possible object allocation of size 24\n"
    backtrace 3
  end
  continue
end
```

### 3. Call Graph Reconstruction

```python
# GDB Python script for call tracing
import gdb

class CallTracer(gdb.Command):
    def __init__(self):
        super(CallTracer, self).__init__("trace-calls", gdb.COMMAND_USER)
        self.call_stack = []
        
    def invoke(self, arg, from_tty):
        gdb.events.stop.connect(self.stop_handler)
        
    def stop_handler(self, event):
        frame = gdb.selected_frame()
        pc = frame.pc()
        
        # Check if it's a call instruction
        inst = gdb.execute("x/i $pc", to_string=True)
        if "call" in inst:
            target = self.get_call_target(inst)
            print(f"Call to: {target} from {hex(pc)}")
            
CallTracer()
```

## Advanced Techniques

### 1. Signature Scanning

```python
# Find common function signatures
def find_string_constructors():
    # Pattern: lea XXX(%rsp),%rdi; call string_ctor
    pattern = [0x48, 0x8d, 0x7c, 0x24]  # lea (%rsp),%rdi
    
    start = 0x400000
    end = 0x500000
    
    for addr in range(start, end):
        if matches_pattern(addr, pattern):
            print(f"Potential string constructor at: {hex(addr)}")
```

### 2. RTTI Extraction

```bash
# Find RTTI structures
(gdb) find /w 0x400000, 0x700000, '_ZTI'

# Examine typeinfo
(gdb) x/s 0x601234
# Might show: "7MyClass"
```

### 3. Heuristic Function Identification

```python
# Identify functions by behavior
def identify_allocation_wrappers():
    # Look for functions that:
    # 1. Call malloc/new
    # 2. Initialize memory
    # 3. Return pointer
    
    # Pattern: call malloc; test result; conditional jump
    malloc_callers = find_calls_to("malloc@plt")
    
    for caller in malloc_callers:
        if has_error_check_after_call(caller):
            print(f"Potential allocation wrapper at: {hex(caller)}")
```

## Case Studies

### Case 1: Debugging std::map Operations

```assembly
# Identifying map insertion
# Look for red-black tree operations
mov    0x18(%rdi),%rax   # _M_node_count
inc    %rax
mov    %rax,0x18(%rdi)

# Tree rotation patterns (left rotate)
mov    0x10(%rsi),%rax   # right child
mov    %rax,0x10(%rdx)   # parent's left = node's right
```

### Case 2: Template Function Identification

```bash
# Templates create multiple versions
# Look for similar code patterns with different types

# Example: vector<int> vs vector<double>
_ZNSt6vectorIiE9push_backERKi    # vector<int>::push_back
_ZNSt6vectorIdE9push_backERKd    # vector<double>::push_back

# The code structure will be nearly identical
```

### Case 3: Lambda Function Detection

```assembly
# Lambda functions often appear as:
# 1. Small functions with captured variables
# 2. Passed to STL algorithms
# 3. Have synthetic names

# Pattern: small function called via function pointer
mov    %rbx,%rdi         # Captured variable access
mov    0x8(%rsp),%rsi    # Lambda parameter
call   *%rax             # Indirect call
```

## Debugging Workflow Summary

1. **Static Analysis**
   - Extract strings and symbols
   - Identify function boundaries
   - Map PLT/GOT entries

2. **Dynamic Analysis**
   - Set strategic breakpoints
   - Trace execution flow
   - Monitor memory allocations

3. **Pattern Recognition**
   - Identify C++ constructs
   - Recognize STL usage
   - Map virtual function calls

4. **Reconstruction**
   - Build call graphs
   - Identify class hierarchies
   - Document findings

## Tools and Resources

### Essential Tools
- **GDB** with Python support
- **objdump** for disassembly
- **readelf** for ELF analysis
- **c++filt** for demangling
- **IDA Pro/Ghidra** for advanced analysis

### Useful Scripts
```bash
# Extract all potential function starts
objdump -d lib.so | grep -E "^[0-9a-f]+ <.*>:" | cut -d' ' -f1

# Find all virtual function calls
objdump -d lib.so | grep -E "call.*\*.*\(%"

# List all string literals
strings -a lib.so | grep -E "^[[:print:]]{10,}$"
```

### References
- Itanium C++ ABI (for name mangling)
- ELF specification
- x86-64 ABI documentation
- C++ object model

Remember: Debugging stripped binaries requires patience, systematic analysis, and pattern recognition. Build your understanding incrementally and document your findings thoroughly.