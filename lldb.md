# LLDB Debugging Guide for x64 Assembly

A comprehensive guide for using LLDB (LLVM Debugger) for assembly-level debugging, with focus on macOS and Linux environments.

## Table of Contents
1. [LLDB vs GDB Comparison](#lldb-vs-gdb-comparison)
2. [Essential LLDB Commands](#essential-lldb-commands)
3. [Assembly Debugging with LLDB](#assembly-debugging-with-lldb)
4. [Advanced LLDB Features](#advanced-lldb-features)
5. [Debugging C++ with LLDB](#debugging-c-with-lldb)
6. [LLDB Python Scripting](#lldb-python-scripting)
7. [Platform-Specific Notes](#platform-specific-notes)

## LLDB vs GDB Comparison

### Command Equivalents

| GDB Command | LLDB Command | Description |
|------------|--------------|-------------|
| `run` | `run` or `r` | Start program |
| `break main` | `breakpoint set -n main` or `b main` | Set breakpoint |
| `step` | `step` or `s` | Step into |
| `next` | `next` or `n` | Step over |
| `stepi` | `si` or `stepi` | Step one instruction |
| `nexti` | `ni` or `nexti` | Next instruction |
| `info registers` | `register read` | Show registers |
| `x/10x $rsp` | `x -c 10 -f x $rsp` | Examine memory |
| `disas main` | `disassemble -n main` | Disassemble function |
| `bt` | `bt` or `thread backtrace` | Show backtrace |
| `print $rax` | `register read rax` or `p $rax` | Print register |
| `set $rax = 10` | `register write rax 10` | Modify register |

## Essential LLDB Commands

### Starting and Running

```bash
# Start LLDB with executable
lldb ./program

# With arguments
lldb -- ./program arg1 arg2

# Attach to running process
lldb -p <pid>
lldb -n <process-name>

# Load core dump
lldb ./program -c core.dump
```

### Breakpoints

```bash
# Set breakpoint by function name
(lldb) breakpoint set --name main
(lldb) br s -n main
(lldb) b main

# Set breakpoint at address
(lldb) breakpoint set --address 0x100000f50
(lldb) br s -a 0x100000f50

# Set conditional breakpoint
(lldb) breakpoint set --name malloc --condition '$rdi == 24'
(lldb) br s -n function -c '$rax > 10'

# List breakpoints
(lldb) breakpoint list
(lldb) br l

# Delete breakpoints
(lldb) breakpoint delete 1
(lldb) br del 1

# Disable/enable breakpoints
(lldb) breakpoint disable 1
(lldb) breakpoint enable 1
```

### Watchpoints

```bash
# Set watchpoint on variable
(lldb) watchpoint set variable global_var
(lldb) wa s v global_var

# Set watchpoint on memory address
(lldb) watchpoint set expression -- 0x100000f50
(lldb) wa s e -- 0x100000f50

# List watchpoints
(lldb) watchpoint list
(lldb) wa l
```

## Assembly Debugging with LLDB

### Disassembly

```bash
# Disassemble current function
(lldb) disassemble
(lldb) di

# Disassemble specific function
(lldb) disassemble --name main
(lldb) di -n main

# Disassemble with mixed source
(lldb) disassemble --mixed
(lldb) di -m

# Disassemble address range
(lldb) disassemble --start-address 0x100000f50 --end-address 0x100000f80
(lldb) di -s 0x100000f50 -e 0x100000f80

# Disassemble N instructions
(lldb) disassemble --pc --count 20
(lldb) di -p -c 20

# Set disassembly flavor (Intel vs AT&T)
(lldb) settings set target.x86-disassembly-flavor intel
(lldb) settings set target.x86-disassembly-flavor att
```

### Register Operations

```bash
# Read all registers
(lldb) register read
(lldb) re r

# Read specific registers
(lldb) register read rax rbx rcx
(lldb) re r rax rbx rcx

# Read with format
(lldb) register read --format binary rflags
(lldb) re r -f b rflags

# Write register
(lldb) register write rax 0x42
(lldb) re w rax 0x42

# Read floating point registers
(lldb) register read --set float
(lldb) re r -s float

# Read AVX registers
(lldb) register read --set avx
```

### Memory Examination

```bash
# Examine memory (similar to GDB's x command)
# Format: memory read [options] <address>

# Read 10 hex words from stack
(lldb) memory read --count 10 --format x --size 8 $rsp
(lldb) x -c 10 -f x -s 8 $rsp
(lldb) x/10xg $rsp  # GDB-style syntax also works

# Read string
(lldb) memory read --format c --size 1 --count 50 0x100000f50
(lldb) x -f c -s 1 -c 50 0x100000f50
(lldb) x/50c 0x100000f50

# Read instructions
(lldb) memory read --format i --count 10 $rip
(lldb) x -f i -c 10 $rip
(lldb) x/10i $rip

# Memory formats:
# x - hexadecimal
# d - decimal  
# u - unsigned decimal
# o - octal
# t - binary
# f - float
# c - char
# s - string
# i - instruction
# a - address
```

### Execution Control

```bash
# Step one instruction
(lldb) thread step-inst
(lldb) si

# Step over one instruction
(lldb) thread step-inst-over
(lldb) ni

# Continue until address
(lldb) thread until 0x100000f80

# Step out of current function
(lldb) thread step-out
(lldb) finish

# Jump to address (changes PC)
(lldb) thread jump --address 0x100000f50
(lldb) jump 0x100000f50
```

## Advanced LLDB Features

### Expression Evaluation

```bash
# Evaluate C/C++ expressions
(lldb) expression -- int $x = 10; $x * 2
(lldb) expr -- (int)malloc(24)
(lldb) e -- *(int*)($rsp + 8)

# Call functions
(lldb) expression -- (void)printf("Hello from LLDB\n")
(lldb) expr -- (int)strlen("test")

# Create convenience variables
(lldb) expr int $myvar = 42
(lldb) expr $myvar += 10
```

### Thread and Frame Management

```bash
# List all threads
(lldb) thread list

# Select thread
(lldb) thread select 2

# Show backtrace for all threads
(lldb) thread backtrace all

# Select frame
(lldb) frame select 2
(lldb) f 2

# Show frame info
(lldb) frame info

# Show local variables
(lldb) frame variable
(lldb) fr v
```

### Image (Module) Commands

```bash
# List loaded images
(lldb) image list

# Show sections of an image
(lldb) image dump sections a.out

# Lookup address
(lldb) image lookup --address 0x100000f50
(lldb) im loo -a 0x100000f50

# Find symbol
(lldb) image lookup --symbol malloc
(lldb) im loo -s malloc

# Show dependencies
(lldb) image list --show-dependent-modules
```

## Debugging C++ with LLDB

### C++ Specific Features

```bash
# Pretty print STL containers
(lldb) expression -- myVector
(lldb) frame variable --show-types myMap

# Set breakpoint in method
(lldb) breakpoint set --method MyClass::myMethod
(lldb) br s -M MyClass::myMethod

# Break on exception throw
(lldb) breakpoint set -E C++
(lldb) br s -E C++

# Print vtable
(lldb) expression -- *(void**)myObject

# Call virtual function
(lldb) expression -- myObject->virtualMethod()
```

### RTTI and Dynamic Type

```bash
# Show dynamic type
(lldb) frame variable --dynamic-type run-target this

# Show RTTI info
(lldb) expression -- typeid(*ptr).name()
```

## LLDB Python Scripting

### Basic Script Example

```python
# save as trace_calls.py
import lldb

def trace_calls(debugger, command, result, internal_dict):
    target = debugger.GetSelectedTarget()
    if not target:
        print("No target selected")
        return
        
    # Set breakpoint on all functions
    for module in target.module_iter():
        for symbol in module:
            if symbol.GetType() == lldb.eSymbolTypeCode:
                address = symbol.GetStartAddress()
                bp = target.BreakpointCreateByAddress(address.GetLoadAddress(target))
                bp.SetScriptCallbackFunction("trace_calls_callback")

def trace_calls_callback(frame, bp_loc, dict):
    function_name = frame.GetFunctionName()
    print(f"Called: {function_name}")
    return False  # Don't stop

def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand('command script add -f trace_calls.trace_calls trace_calls')
```

### Loading Scripts

```bash
# Load Python script
(lldb) command script import ~/trace_calls.py

# Run custom command
(lldb) trace_calls
```

### Synthetic Child Providers

```python
# Custom formatter for your types
class MyClassChildProvider:
    def __init__(self, valobj, internal_dict):
        self.valobj = valobj
        
    def num_children(self):
        return 3
        
    def get_child_at_index(self, index):
        if index == 0:
            return self.valobj.GetChildMemberWithName('member1')
        # ... etc

# Register the provider
def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand('type synthetic add -l trace_calls.MyClassChildProvider MyClass')
```

## Platform-Specific Notes

### macOS Specific

```bash
# Deal with System Integrity Protection (SIP)
# May need to use sudo or disable SIP for system processes

# Code signing for debugging
codesign -s - -f --entitlements entitlements.plist ./program

# Entitlements file content:
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.get-task-allow</key>
    <true/>
</dict>
</plist>
```

### Linux Specific

```bash
# May need to set ptrace permissions
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope

# For debugging stripped binaries
(lldb) settings set target.exec-search-paths /usr/lib/debug
```

### Universal Commands

```bash
# Set up common preferences
(lldb) settings set target.x86-disassembly-flavor intel
(lldb) settings set target.skip-prologue false
(lldb) settings set target.max-string-summary-length 1024

# Save settings
(lldb) settings write -f ~/.lldbinit
```

## Assembly Debugging Workflow

### 1. Initial Setup

```bash
# Start LLDB
lldb ./program

# Set Intel syntax
(lldb) settings set target.x86-disassembly-flavor intel

# Set breakpoint at entry
(lldb) b main
(lldb) r
```

### 2. Exploration

```bash
# Show current location
(lldb) disassemble --pc --count 20

# Examine registers
(lldb) register read

# Check stack
(lldb) x/20xg $rsp
```

### 3. Tracing Execution

```bash
# Step through instructions
(lldb) si
(lldb) register read rax rdi rsi

# Set watchpoint
(lldb) watchpoint set expression -- *(int*)0x100000000
```

### 4. Advanced Analysis

```python
# Custom stepping script
def step_until_call(debugger, command, result, internal_dict):
    target = debugger.GetSelectedTarget()
    process = target.GetProcess()
    thread = process.GetSelectedThread()
    
    while True:
        thread.StepInstruction(False)
        frame = thread.GetSelectedFrame()
        disassembly = frame.Disassemble()
        
        if 'call' in disassembly:
            print(f"Stopped at call: {disassembly}")
            break
```

## Common LLDB Aliases

Add to `~/.lldbinit`:

```bash
# GDB compatibility aliases
command alias gdb-info-registers register read
command alias gdb-x memory read
command alias gdb-disas disassemble

# Convenience aliases
command alias rs register read
command alias rw register write
command alias mm memory read
command alias mw memory write
command alias dc disassemble --count

# Assembly debugging aliases
command alias regs register read rax rbx rcx rdx rsi rdi rbp rsp rip
command alias stack x/20xg $rsp
command alias code disassemble --pc --count 10
```

## Tips and Tricks

1. **Use command completion**: Tab completion works for most commands
2. **Command abbreviations**: Most commands can be abbreviated (e.g., `br` for `breakpoint`)
3. **Regex breakpoints**: `breakpoint set -r "^prefix_.*"` sets breakpoints on all functions starting with "prefix_"
4. **Format strings**: Use `${var.member}` in format strings
5. **History**: Use `↑` and `↓` to navigate command history
6. **Scripting**: LLDB's Python API is more powerful than command-line for complex tasks

## Troubleshooting

### Common Issues

1. **"Unable to find process"**: Check permissions, may need sudo
2. **No debug symbols**: Use `add-dsym` command or `target symbols add`
3. **Optimized code**: Use `-O0` when compiling for easier debugging
4. **Wrong architecture**: Ensure LLDB matches target architecture

### Debug Output

```bash
# Enable LLDB logging
(lldb) log enable lldb all
(lldb) log enable gdb-remote packets

# Save log to file
(lldb) log enable -f /tmp/lldb.log lldb all
```

Remember: LLDB is particularly powerful on macOS/iOS development but works well cross-platform. Its Python API makes it excellent for automated debugging tasks.