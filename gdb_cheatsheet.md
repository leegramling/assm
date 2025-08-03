# GDB Cheat Sheet for x64 Assembly Debugging

## Essential Setup Commands

```bash
# Start GDB with executable
gdb ./program

# Set Intel syntax (more readable)
(gdb) set disassembly-flavor intel

# Enable pretty printing
(gdb) set print pretty on

# Save command history
(gdb) set history save on
```

## Breakpoints and Execution Control

```bash
# Set breakpoint at function
(gdb) break main
(gdb) b function_name

# Set breakpoint at address
(gdb) break *0x401234
(gdb) b *$rip+10

# List breakpoints
(gdb) info breakpoints
(gdb) info b

# Delete breakpoints
(gdb) delete 1          # Delete breakpoint 1
(gdb) clear             # Delete all breakpoints

# Conditional breakpoints
(gdb) break *0x401234 if $rax == 0x42

# Run program
(gdb) run
(gdb) run arg1 arg2     # With arguments

# Continue execution
(gdb) continue
(gdb) c

# Step one instruction
(gdb) stepi
(gdb) si

# Step over (don't enter calls)
(gdb) nexti
(gdb) ni

# Finish current function
(gdb) finish
```

## Register Examination

```bash
# Show all registers
(gdb) info registers
(gdb) info all-registers    # Including FPU/SSE

# Print specific register
(gdb) print $rax
(gdb) p/x $rax             # Hexadecimal
(gdb) p/d $rax             # Decimal
(gdb) p/t $rax             # Binary

# Modify register
(gdb) set $rax = 0x42

# Show flags register
(gdb) print $eflags
(gdb) p/t $eflags          # See individual flags

# Common flags positions:
# CF (Carry): bit 0
# ZF (Zero): bit 6
# SF (Sign): bit 7
# OF (Overflow): bit 11
```

## Memory Examination

```bash
# Examine memory
(gdb) x/[count][format][size] address

# Formats:
# x - hex
# d - decimal
# u - unsigned decimal
# o - octal
# t - binary
# f - float
# s - string
# i - instruction

# Sizes:
# b - byte
# h - halfword (2 bytes)
# w - word (4 bytes)
# g - giant (8 bytes)

# Examples:
(gdb) x/10xw $rsp          # 10 words in hex from stack
(gdb) x/8xg $rsp           # 8 qwords from stack
(gdb) x/s 0x401234         # String at address
(gdb) x/10i $rip           # 10 instructions from RIP
(gdb) x/20xb $rdi          # 20 bytes from RDI

# Display memory mappings
(gdb) info proc mappings
```

## Disassembly

```bash
# Disassemble function
(gdb) disassemble main
(gdb) disas function_name

# Disassemble with source
(gdb) disas /m main

# Disassemble current location
(gdb) disas $rip

# Disassemble range
(gdb) disas 0x401000,0x401100
(gdb) disas $rip,+20

# Show next instructions
(gdb) x/10i $rip
```

## Stack Analysis

```bash
# Show backtrace
(gdb) backtrace
(gdb) bt
(gdb) bt full              # With local variables

# Select stack frame
(gdb) frame 2
(gdb) f 2

# Show frame info
(gdb) info frame
(gdb) info args            # Function arguments
(gdb) info locals          # Local variables

# Stack examination
(gdb) x/20xg $rsp          # Stack contents
(gdb) print $rbp - $rsp    # Stack frame size
```

## Watchpoints

```bash
# Watch for variable changes
(gdb) watch variable_name
(gdb) watch *0x601234      # Watch memory address

# Watch for reads
(gdb) rwatch *0x601234

# Watch for access (read or write)
(gdb) awatch *0x601234

# List watchpoints
(gdb) info watchpoints
```

## C++ Specific Debugging

```bash
# Print object
(gdb) print *this
(gdb) print object

# Show vtable
(gdb) info vtbl object
(gdb) x/10xg *(void**)&object  # Manual vtable inspection

# Demangle C++ names
(gdb) set print demangle on
(gdb) info functions       # Shows demangled names

# Call C++ methods
(gdb) call object.method()
```

## Assembly-Specific Tips

```bash
# Track execution flow
(gdb) display/10i $rip     # Always show next 10 instructions
(gdb) display $rax         # Always show RAX value

# Trace system calls
(gdb) catch syscall

# Find instruction patterns
(gdb) find /b 0x400000, 0x500000, 0x90  # Find NOP (0x90)

# Show sections
(gdb) info files
(gdb) maintenance info sections
```

## Advanced Techniques

```bash
# Log output to file
(gdb) set logging on
(gdb) set logging file debug.log

# Execute commands on breakpoint
(gdb) commands 1
> print $rax
> continue
> end

# Python scripting
(gdb) python print(gdb.parse_and_eval("$rax"))

# Reverse debugging (if supported)
(gdb) record
(gdb) reverse-stepi
(gdb) reverse-continue
```

## Common x64 Assembly Patterns

```bash
# Function prologue
push %rbp
mov %rsp, %rbp

# Function epilogue
pop %rbp
ret

# Call instruction analysis
(gdb) x/i $rip            # See call target
(gdb) info symbol 0x401234  # Get symbol name

# Virtual function call
mov (%rdi), %rax          # Load vtable
call *0x8(%rax)           # Call second function
```

## Debugging Optimized Code

```bash
# Common optimization indicators:
# - Missing frame pointers
# - Inlined functions
# - Reordered instructions
# - Eliminated variables

# Tips:
(gdb) info locals         # May show <optimized out>
(gdb) disas /m           # Mix source and assembly
(gdb) set print frame-arguments all
```

## Quick Analysis Commands

```bash
# Find string references
(gdb) info functions
(gdb) info variables

# Search memory
(gdb) find /w 0x400000, 0x500000, 0xdeadbeef

# Show all registers with values
(gdb) macro define show_regs
> info registers rax rbx rcx rdx rsi rdi rbp rsp rip
> end
(gdb) show_regs
```

## Useful .gdbinit Configuration

```bash
# Add to ~/.gdbinit
set disassembly-flavor intel
set print pretty on
set history save on

# Custom command for assembly debugging
define asm
  display/10i $rip
  display $rax
  display $rdi
  display $rsi
end

# Hook to show registers on stop
define hook-stop
  x/1i $rip
end
```

## Emergency Commands

```bash
# When GDB is unresponsive
Ctrl+C                    # Interrupt execution

# Quit GDB
(gdb) quit
(gdb) q

# Kill program
(gdb) kill

# Detach from process
(gdb) detach
```

This cheat sheet covers the most common GDB commands for x64 assembly debugging. Keep it handy during debugging sessions!