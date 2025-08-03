# tutMasm.md - Windows MASM x64 Assembly and Debugging Tutorial

This tutorial teaches x64 assembly programming using Microsoft MASM (Macro Assembler) and debugging with Visual Studio debugger and WinDbg. Learn to debug Windows applications and libraries when only assembly is visible.

## Prerequisites

- Visual Studio with MSVC tools
- Windows SDK
- Basic understanding of programming concepts
- Windows command line familiarity

## Development Environment Setup

### Required Tools
```cmd
# Visual Studio Developer Command Prompt provides:
- ml64.exe (MASM 64-bit assembler)
- link.exe (Microsoft Linker)  
- dumpbin.exe (PE file analyzer)
- cl.exe (C/C++ compiler for mixed projects)

# Debugging Tools
- Visual Studio Debugger
- WinDbg (Windows Debugger)
- x64dbg (third-party debugger)
```

### Build Environment
```cmd
# Set up environment (if not using VS Developer Command Prompt)
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

---

# Tutorial 1: Basic Registers and MOV Instructions

## Lesson: Understanding x64 Registers in Windows Environment

### x64 Register Architecture
Windows x64 uses the same register set as System V, but with different calling conventions:

```
64-bit Registers (Intel Syntax):
RAX, RBX, RCX, RDX, RSI, RDI, RBP, RSP, R8-R15

32-bit portions: EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP, R8D-R15D
16-bit portions: AX, BX, CX, DX, SI, DI, BP, SP, R8W-R15W  
8-bit portions:  AL, BL, CL, DL, SIL, DIL, BPL, SPL, R8B-R15B
```

### Windows x64 Calling Convention
Unlike System V ABI, Windows uses a different parameter passing:

```assembly
; Windows x64 calling convention:
; RCX = 1st parameter
; RDX = 2nd parameter  
; R8  = 3rd parameter
; R9  = 4th parameter
; Additional parameters on stack (right to left)
; RAX = return value
; Caller must allocate 32 bytes "shadow space" on stack
```

### MOV Instruction Fundamentals
The MOV instruction copies data between registers, memory, and immediate values:

```assembly
mov rax, 42           ; Load immediate value 42 into RAX
mov rax, rbx          ; Copy RBX contents to RAX
mov rax, [rbx]        ; Load memory value at address in RBX into RAX
mov [rbx], rax        ; Store RAX value to memory at address in RBX
mov rax, [rbx + 8]    ; Load from RBX + 8 offset
```

### Data Movement Examples

#### Immediate to Register
```assembly
mov rax, 0x123456789ABCDEF0    ; 64-bit immediate
mov eax, 0x12345678            ; 32-bit immediate (zeros upper 32 bits)
mov ax, 0x1234                 ; 16-bit immediate  
mov al, 0x12                   ; 8-bit immediate
```

#### Register to Register
```assembly
mov rax, rbx          ; 64-bit copy
mov eax, ebx          ; 32-bit copy (zeros upper 32 bits of RAX)
mov ax, bx            ; 16-bit copy (preserves upper bits of RAX)
mov al, bl            ; 8-bit copy (preserves upper bits of RAX)
```

#### Memory Operations
```assembly
mov rax, [rcx]                 ; Load 8 bytes from memory at RCX
mov eax, [rcx]                 ; Load 4 bytes from memory at RCX
mov ax, [rcx]                  ; Load 2 bytes from memory at RCX
mov al, [rcx]                  ; Load 1 byte from memory at RCX

mov [rcx], rax                 ; Store 8 bytes to memory at RCX
mov [rcx], eax                 ; Store 4 bytes to memory at RCX
mov [rcx], ax                  ; Store 2 bytes to memory at RCX
mov [rcx], al                  ; Store 1 byte to memory at RCX
```

### Step-by-Step Register Usage

#### Example: Data Manipulation
```assembly
; Load values
mov rax, 100                   ; RAX = 100
mov rbx, 200                   ; RBX = 200

; Copy data
mov rcx, rax                   ; RCX = 100 (copy of RAX)
mov rdx, rbx                   ; RDX = 200 (copy of RBX)

; Work with different sizes
mov eax, 0x12345678            ; EAX = 0x12345678, upper 32 bits of RAX = 0
mov ax, 0x9ABC                 ; AX = 0x9ABC, RAX = 0x00000000_00009ABC
mov al, 0xDE                   ; AL = 0xDE, RAX = 0x00000000_00009ADE
```

### Zero Extension vs Sign Extension
Understanding how different operation sizes affect registers:

```assembly
; Zero extension (automatic with 32-bit operations)
mov rax, 0xFFFFFFFFFFFFFFFF    ; RAX = all 1s
mov eax, 0x12345678            ; RAX = 0x00000000_12345678 (upper 32 bits zeroed)

; 16-bit and 8-bit operations preserve upper bits
mov rax, 0xFFFFFFFFFFFFFFFF    ; RAX = all 1s  
mov ax, 0x1234                 ; RAX = 0xFFFFFFFF_FFFF1234 (upper bits unchanged)
mov al, 0x56                   ; RAX = 0xFFFFFFFF_FFFF1256 (only AL changed)
```

### Memory Addressing Modes
x64 supports sophisticated addressing:

```assembly
mov rax, [rbx]                 ; Direct: [base]
mov rax, [rbx + 8]             ; Base + displacement: [base + disp]
mov rax, [rbx + rcx]           ; Base + index: [base + index]  
mov rax, [rbx + rcx*4]         ; Base + scaled index: [base + index*scale]
mov rax, [rbx + rcx*4 + 8]     ; Full: [base + index*scale + disp]
```

**Scale factors:** 1, 2, 4, or 8 (perfect for array indexing)

### Windows-Specific Considerations

#### Structured Exception Handling (SEH)
When accessing memory, Windows can generate exceptions:

```assembly
; This might cause access violation
mov rax, [0]                   ; Accessing null pointer

; Proper error handling requires SEH or bounds checking
```

#### Thread Information Block (TIB) Access
```assembly
; Access Thread Information Block via GS segment
mov rax, gs:[0x30]             ; Get Process Environment Block (PEB)
mov rax, gs:[0x08]             ; Get stack base
mov rax, gs:[0x10]             ; Get stack limit
```

## Problem
Implement basic data movement and register manipulation using Windows MASM.

### Skills Covered
- Register operations and data sizes
- Memory addressing modes
- Windows x64 calling convention
- Immediate value loading

### Your Task
Create a MASM program that demonstrates register usage:

**tutorial1.asm**:
```assembly
; Tutorial 1: Basic registers and MOV instructions
.code

ExitProcess PROTO
printf PROTO

.data
    value1 dq 0x123456789ABCDEF0
    value2 dq 100
    result dq ?
    format_str db 'RAX: %016llX, RBX: %016llX, Result: %lld', 0Dh, 0Ah, 0

.code
main PROC
    ; TODO: Implement register operations
    ; 1. Load value1 into RAX
    ; 2. Load value2 into RBX  
    ; 3. Copy RAX to RCX
    ; 4. Store RCX to result
    ; 5. Print results using printf
    
    ; Example framework:
    mov rax, value1            ; Load first value
    mov rbx, value2            ; Load second value
    ; Add your code here...
    
    ; Print results
    lea rcx, format_str        ; Format string
    mov rdx, rax               ; First value
    mov r8, rbx                ; Second value
    mov r9, result             ; Third value
    call printf
    
    ; Exit
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Build commands:**
```cmd
ml64 tutorial1.asm /link /subsystem:console /entry:main msvcrt.lib kernel32.lib
```

**Expected Output:** Register values and memory operations demonstration

---

# Tutorial 2: Arithmetic Operations and Flags

## Lesson: x64 Arithmetic Instructions and RFLAGS Register

### Arithmetic Instructions
x64 provides comprehensive arithmetic operations:

```assembly
add rax, rbx          ; RAX = RAX + RBX
sub rax, rbx          ; RAX = RAX - RBX  
imul rax, rbx         ; RAX = RAX * RBX (signed)
mul rbx               ; RDX:RAX = RAX * RBX (unsigned, 128-bit result)
idiv rbx              ; RAX = RAX / RBX, RDX = RAX % RBX (signed)
div rbx               ; RAX = RAX / RBX, RDX = RAX % RBX (unsigned)
```

### The RFLAGS Register
RFLAGS contains status flags that reflect the result of operations:

```
RFLAGS Register (relevant flags for arithmetic):
Bit 0:  CF (Carry Flag) - Set on unsigned overflow
Bit 2:  PF (Parity Flag) - Set if result has even number of 1s in low byte
Bit 6:  ZF (Zero Flag) - Set if result is zero
Bit 7:  SF (Sign Flag) - Set if result is negative (MSB = 1)
Bit 11: OF (Overflow Flag) - Set on signed overflow
```

### Flag-Setting Operations

#### Addition and Flags
```assembly
mov rax, 0xFFFFFFFFFFFFFFFF    ; RAX = maximum unsigned value
add rax, 1                     ; RAX = 0, CF = 1 (carry), ZF = 1 (zero)

mov rax, 0x7FFFFFFFFFFFFFFF    ; RAX = maximum signed positive
add rax, 1                     ; RAX = 0x8000000000000000, OF = 1 (overflow)
```

#### Subtraction and Flags
```assembly
mov rax, 5
sub rax, 10                    ; RAX = -5, SF = 1 (negative), CF = 1 (borrow)

mov rax, 10  
sub rax, 10                    ; RAX = 0, ZF = 1 (zero)
```

#### Multiplication Considerations
```assembly
; Signed multiplication
mov rax, -5
mov rbx, 3
imul rax, rbx                  ; RAX = -15

; Unsigned multiplication with full result
mov rax, 0xFFFFFFFFFFFFFFFF
mov rbx, 2
mul rbx                        ; RDX:RAX = 0x1_FFFFFFFFFFFFFFFE
```

### Division Operations

#### Signed Division Setup
```assembly
; For IDIV, must sign-extend RAX to RDX:RAX
mov rax, -100                  ; Dividend
cqo                            ; Sign-extend RAX to RDX:RAX
mov rbx, 7                     ; Divisor
idiv rbx                       ; RAX = -14, RDX = -2 (quotient, remainder)
```

#### Unsigned Division Setup
```assembly
; For DIV, must zero RDX
mov rax, 100                   ; Dividend
xor rdx, rdx                   ; Clear RDX (or use mov rdx, 0)
mov rbx, 7                     ; Divisor
div rbx                        ; RAX = 14, RDX = 2 (quotient, remainder)
```

### Increment and Decrement
```assembly
inc rax                        ; RAX++, affects all flags except CF
dec rax                        ; RAX--, affects all flags except CF

; Equivalent operations:
add rax, 1                     ; Same as INC but affects CF
sub rax, 1                     ; Same as DEC but affects CF
```

### Comparison Operations
CMP performs subtraction but doesn't store the result, only sets flags:

```assembly
mov rax, 10
mov rbx, 5
cmp rax, rbx                   ; Compare RAX with RBX (10 - 5)
                               ; ZF = 0 (not equal), SF = 0 (positive result)

cmp rax, rax                   ; Compare with itself
                               ; ZF = 1 (equal)
```

### Logical Operations
```assembly
and rax, rbx                   ; Bitwise AND
or rax, rbx                    ; Bitwise OR
xor rax, rbx                   ; Bitwise XOR
not rax                        ; Bitwise NOT (one's complement)
neg rax                        ; Two's complement negation
```

### Step-by-Step: Calculator Implementation
```assembly
; Simple calculator: (a + b) * c - d
mov rax, 10                    ; a = 10
mov rbx, 5                     ; b = 5
mov rcx, 3                     ; c = 3
mov rdx, 7                     ; d = 7

; Step 1: a + b
add rax, rbx                   ; RAX = 15

; Step 2: (a + b) * c
imul rax, rcx                  ; RAX = 45

; Step 3: ((a + b) * c) - d
sub rax, rdx                   ; RAX = 38
```

### Overflow Detection
```assembly
; Detecting signed overflow
mov rax, 0x7FFFFFFFFFFFFFFF    ; Largest positive 64-bit number
add rax, 1                     ; This causes signed overflow
jo overflow_handler            ; Jump if overflow flag set

; Detecting unsigned overflow (carry)
mov rax, 0xFFFFFFFFFFFFFFFF    ; Largest unsigned 64-bit number
add rax, 1                     ; This causes unsigned overflow
jc carry_handler               ; Jump if carry flag set

overflow_handler:
    ; Handle signed overflow
    jmp continue

carry_handler:
    ; Handle unsigned overflow  
    jmp continue

continue:
    ; Normal execution continues
```

### Windows-Specific: Using MASM Expressions
```assembly
; MASM supports compile-time arithmetic
VALUE1 equ 100
VALUE2 equ 50
RESULT equ VALUE1 + VALUE2     ; RESULT = 150

.data
    computed_value dq RESULT   ; Stores 150
```

### Advanced Arithmetic: Extended Precision
```assembly
; 128-bit addition using carry
mov rax, 0xFFFFFFFFFFFFFFFF    ; Low part of first number
mov rbx, 0x1234567890ABCDEF0   ; High part of first number
mov rcx, 0x0000000000000001    ; Low part of second number  
mov rdx, 0x0000000000000000    ; High part of second number

add rax, rcx                   ; Add low parts
adc rbx, rdx                   ; Add high parts with carry
; Result: RBX:RAX contains 128-bit sum
```

## Problem
Implement arithmetic operations with proper flag handling.

### Skills Covered
- Addition, subtraction, multiplication, division
- Flag register understanding
- Overflow detection
- Extended precision arithmetic

### Your Task
Create a comprehensive arithmetic program:

**tutorial2.asm**:
```assembly
; Tutorial 2: Arithmetic operations and flags
.code

ExitProcess PROTO
printf PROTO

.data
    num1 dq 0x7FFFFFFFFFFFFFFF    ; Large positive number
    num2 dq 2                     ; Multiplier
    dividend dq 100               ; For division
    divisor dq 7                  ; Divisor
    
    overflow_msg db 'Overflow detected!', 0Dh, 0Ah, 0
    result_msg db 'Results: Add=%lld, Mul=%lld, Div=%lld, Rem=%lld', 0Dh, 0Ah, 0

.code
main PROC
    ; TODO: Implement arithmetic operations
    ; 1. Add num1 + 1 (check for overflow)  
    ; 2. Multiply num1 * num2
    ; 3. Divide dividend by divisor
    ; 4. Handle overflow conditions
    ; 5. Print results
    
    ; Example framework:
    mov rax, num1
    add rax, 1
    jo handle_overflow            ; Jump if overflow
    
    ; Continue with other operations...
    
    ; Exit
    mov rcx, 0
    call ExitProcess

handle_overflow:
    ; Print overflow message
    lea rcx, overflow_msg
    call printf
    jmp exit_program

exit_program:
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Expected Output:** Arithmetic results with overflow detection

---

# Tutorial 3: Control Flow and Jumps

## Lesson: Conditional and Unconditional Control Flow

### Jump Instructions
x64 provides various jump instructions for control flow:

#### Unconditional Jumps
```assembly
jmp label                      ; Always jump to label
jmp rax                        ; Jump to address in RAX
jmp qword ptr [rax]            ; Jump to address stored at [RAX]
```

#### Conditional Jumps (based on flags)
```assembly
; Zero Flag (ZF)
jz label                       ; Jump if Zero (ZF = 1)
jnz label                      ; Jump if Not Zero (ZF = 0)
je label                       ; Jump if Equal (same as JZ)
jne label                      ; Jump if Not Equal (same as JNZ)

; Carry Flag (CF) - for unsigned comparisons  
jc label                       ; Jump if Carry (CF = 1)
jnc label                      ; Jump if No Carry (CF = 0)
jb label                       ; Jump if Below (unsigned <)
jbe label                      ; Jump if Below or Equal (unsigned <=)
ja label                       ; Jump if Above (unsigned >)
jae label                      ; Jump if Above or Equal (unsigned >=)

; Sign Flag (SF) and Overflow Flag (OF) - for signed comparisons
js label                       ; Jump if Sign (SF = 1, negative)
jns label                      ; Jump if No Sign (SF = 0, positive/zero)
jl label                       ; Jump if Less (signed <)
jle label                      ; Jump if Less or Equal (signed <=)
jg label                       ; Jump if Greater (signed >)
jge label                      ; Jump if Greater or Equal (signed >=)

; Overflow Flag (OF)
jo label                       ; Jump if Overflow (OF = 1)
jno label                      ; Jump if No Overflow (OF = 0)
```

### Conditional Execution Pattern
The typical pattern: **Compare, then Jump**

```assembly
; Compare two values
mov rax, 10
mov rbx, 5
cmp rax, rbx                   ; Compare RAX with RBX (10 - 5)

; Jump based on comparison
jg greater_label               ; Jump if RAX > RBX
je equal_label                 ; Jump if RAX = RBX  
jl less_label                  ; Jump if RAX < RBX

greater_label:
    ; RAX is greater than RBX
    jmp continue

equal_label:
    ; RAX equals RBX
    jmp continue

less_label:
    ; RAX is less than RBX
    jmp continue

continue:
    ; Execution continues here
```

### Loop Constructs

#### Simple Loop with Counter
```assembly
; Count from 1 to 10
mov rcx, 10                    ; Loop counter

loop_start:
    ; Loop body - process RCX
    push rcx                   ; Save counter
    ; ... do work with current count ...
    pop rcx                    ; Restore counter
    
    dec rcx                    ; Decrement counter
    jnz loop_start             ; Continue if not zero
```

#### Loop with Array Processing
```assembly
; Process array elements
mov rsi, offset array_data     ; Source pointer
mov rcx, array_size            ; Element count
xor rax, rax                   ; Accumulator (sum)

process_loop:
    add rax, qword ptr [rsi]   ; Add current element to sum
    add rsi, 8                 ; Move to next element (8 bytes)
    dec rcx                    ; Decrement count
    jnz process_loop           ; Continue if more elements
    
; RAX now contains sum of all elements
```

### If-Then-Else Structures

#### Simple If Statement
```assembly
; if (rax > 100) { rax = rax * 2; }
cmp rax, 100
jle skip_multiply              ; Jump if RAX <= 100

; True branch
shl rax, 1                     ; Multiply by 2 (left shift)

skip_multiply:
    ; Continue execution
```

#### If-Then-Else Structure
```assembly
; if (rax > rbx) { rcx = 1; } else { rcx = 0; }
cmp rax, rbx
jg set_true                    ; Jump if RAX > RBX

; Else branch
mov rcx, 0                     ; Set false
jmp continue

set_true:
    mov rcx, 1                 ; Set true

continue:
    ; RCX now contains comparison result
```

#### Nested If Statements
```assembly
; if (rax > 0) {
;     if (rax < 100) {
;         rbx = rax * 2;
;     } else {
;         rbx = rax / 2;  
;     }
; } else {
;     rbx = 0;
; }

cmp rax, 0
jle else_outer                 ; Jump if RAX <= 0

; Outer if true: rax > 0
    cmp rax, 100
    jge inner_else             ; Jump if RAX >= 100
    
    ; Inner if true: 0 < rax < 100
    mov rbx, rax
    shl rbx, 1                 ; rbx = rax * 2
    jmp continue
    
    inner_else:
        ; Inner else: rax >= 100
        mov rbx, rax
        shr rbx, 1             ; rbx = rax / 2
        jmp continue

else_outer:
    ; Outer else: rax <= 0
    mov rbx, 0

continue:
    ; Execution continues
```

### Switch/Case Implementation

#### Jump Table Approach
```assembly
; switch (rax) {
;     case 0: ... break;
;     case 1: ... break;
;     case 2: ... break;
;     default: ... break;
; }

.data
    jump_table dq case_0, case_1, case_2

.code
    cmp rax, 2                     ; Check bounds
    ja default_case                ; Jump if above 2
    
    jmp qword ptr [jump_table + rax*8]  ; Jump via table
    
case_0:
    ; Handle case 0
    jmp switch_end
    
case_1:
    ; Handle case 1  
    jmp switch_end
    
case_2:
    ; Handle case 2
    jmp switch_end
    
default_case:
    ; Handle default case
    
switch_end:
    ; Continue execution
```

### Short-Circuit Evaluation

#### Logical AND
```assembly
; if (condition1 && condition2) { ... }
cmp rax, 0
je skip_second                 ; Skip second test if first is false

cmp rbx, 0                     ; Second condition
je skip_action                 ; Skip if second is false

; Both conditions true
    ; ... action code ...

skip_second:
skip_action:
    ; Continue
```

#### Logical OR
```assembly
; if (condition1 || condition2) { ... }
cmp rax, 0
jne do_action                  ; Do action if first is true

cmp rbx, 0                     ; Second condition
je skip_action                 ; Skip if second is also false

do_action:
    ; At least one condition was true
    ; ... action code ...

skip_action:
    ; Continue
```

### Loop Optimization Techniques

#### Loop Unrolling
```assembly
; Process 4 elements at once
mov rsi, offset array_data
mov rcx, element_count
shr rcx, 2                     ; Divide by 4 for unrolled loop

unrolled_loop:
    mov rax, qword ptr [rsi]       ; Element 0
    add rax, qword ptr [rsi + 8]   ; Element 1  
    add rax, qword ptr [rsi + 16]  ; Element 2
    add rax, qword ptr [rsi + 24]  ; Element 3
    ; Process 4 elements worth of data
    
    add rsi, 32                    ; Move to next group of 4
    dec rcx
    jnz unrolled_loop
```

### Windows Exception Handling Integration
```assembly
; Structured Exception Handling aware jumps
; Use when accessing potentially invalid memory

.data
    safe_address dq ?

.code
    ; This could generate access violation
    mov rax, qword ptr [0]         ; Dangerous memory access
    mov safe_address, rax
    jmp success

    ; SEH handler would redirect here on exception
exception_handler:
    mov safe_address, 0            ; Safe default value
    
success:
    ; Continue with safe_address value
```

## Problem
Implement control flow structures using conditional jumps.

### Skills Covered
- Conditional and unconditional jumps
- Loop implementation
- If-then-else structures
- Switch/case constructs

### Your Task
Create control flow demonstration program:

**tutorial3.asm**:
```assembly
; Tutorial 3: Control flow and jumps
.code

ExitProcess PROTO
printf PROTO

.data
    numbers dq 15, 8, 23, 4, 16, 42, 7, 31
    count equ ($ - numbers) / 8
    
    max_val dq ?
    min_val dq ?
    even_count dq 0
    odd_count dq 0
    
    result_msg db 'Max: %lld, Min: %lld, Even: %lld, Odd: %lld', 0Dh, 0Ah, 0

.code
main PROC
    ; TODO: Implement control flow
    ; 1. Find maximum value in array
    ; 2. Find minimum value in array  
    ; 3. Count even and odd numbers
    ; 4. Use loops and conditional jumps
    
    ; Initialize
    mov rsi, offset numbers        ; Array pointer
    mov rcx, count                 ; Element count
    mov rax, qword ptr [rsi]       ; First element for min/max
    mov max_val, rax
    mov min_val, rax
    
find_loop:
    ; TODO: Compare current element with max/min
    ; TODO: Check if even or odd
    ; TODO: Update counters
    
    add rsi, 8                     ; Next element
    dec rcx
    jnz find_loop
    
    ; Print results
    lea rcx, result_msg
    mov rdx, max_val
    mov r8, min_val
    mov r9, even_count
    mov rax, odd_count
    mov qword ptr [rsp + 32], rax
    call printf
    
    ; Exit
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Expected Output:** Statistical analysis of array using control flow

---

# Tutorial 4: Function Calls and Stack Manipulation

## Lesson: Windows x64 Calling Convention and Stack Management

### Windows x64 Calling Convention
Windows uses a different calling convention than System V:

```assembly
; Parameter passing:
; RCX = 1st parameter (integer/pointer)
; RDX = 2nd parameter (integer/pointer)  
; R8  = 3rd parameter (integer/pointer)
; R9  = 4th parameter (integer/pointer)
; XMM0 = 1st floating point parameter
; XMM1 = 2nd floating point parameter
; XMM2 = 3rd floating point parameter  
; XMM3 = 4th floating point parameter
; Stack = 5th+ parameters (right to left)

; Return values:
; RAX = integer/pointer return
; XMM0 = floating point return

; Shadow space:
; Caller must allocate 32 bytes on stack for callee's use
```

### Stack Frame Structure
Windows x64 stack frames have specific requirements:

```
High Memory
┌─────────────────┐
│ Parameter N     │ ← RSP + 48 + 8*(N-5) for 5th+ parameters
│ Parameter 6     │ ← RSP + 48  
│ Parameter 5     │ ← RSP + 40
│ Shadow space R9 │ ← RSP + 32 (4th parameter shadow)
│ Shadow space R8 │ ← RSP + 24 (3rd parameter shadow)
│ Shadow space RDX│ ← RSP + 16 (2nd parameter shadow)  
│ Shadow space RCX│ ← RSP + 8  (1st parameter shadow)
│ Return address  │ ← RSP (after CALL instruction)
└─────────────────┘
Low Memory
```

### Function Prologue and Epilogue

#### Standard Prologue
```assembly
my_function PROC
    ; Standard prologue
    push rbp                       ; Save caller's frame pointer
    mov rbp, rsp                   ; Set up our frame pointer
    sub rsp, 32                    ; Allocate shadow space (minimum)
    sub rsp, local_space           ; Allocate space for local variables
    
    ; Function body goes here
    
    ; Standard epilogue  
    add rsp, local_space + 32      ; Deallocate local space + shadow space
    pop rbp                        ; Restore caller's frame pointer
    ret                            ; Return to caller
my_function ENDP
```

#### Simplified Epilogue with LEAVE
```assembly
my_function PROC
    push rbp
    mov rbp, rsp
    sub rsp, space_needed
    
    ; Function body
    
    leave                          ; Equivalent to: mov rsp, rbp; pop rbp  
    ret
my_function ENDP
```

### Parameter Passing Examples

#### Simple Function Call
```assembly
; Call function with 3 parameters: func(10, 20, 30)
mov rcx, 10                        ; 1st parameter
mov rdx, 20                        ; 2nd parameter  
mov r8, 30                         ; 3rd parameter
call my_function

; Function definition
my_function PROC
    ; RCX = 10, RDX = 20, R8 = 30
    add rcx, rdx                   ; RCX = 30
    add rcx, r8                    ; RCX = 60  
    mov rax, rcx                   ; Return sum in RAX
    ret
my_function ENDP
```

#### Function with Many Parameters
```assembly
; Call func(1, 2, 3, 4, 5, 6, 7)
sub rsp, 32                        ; Allocate shadow space
mov qword ptr [rsp + 40], 7        ; 7th parameter on stack
mov qword ptr [rsp + 32], 6        ; 6th parameter on stack  
mov qword ptr [rsp + 24], 5        ; 5th parameter on stack
mov rcx, 1                         ; 1st parameter
mov rdx, 2                         ; 2nd parameter
mov r8, 3                          ; 3rd parameter
mov r9, 4                          ; 4th parameter
call many_param_func
add rsp, 32                        ; Clean up shadow space

many_param_func PROC
    push rbp
    mov rbp, rsp
    
    ; Access parameters:
    ; RCX = 1, RDX = 2, R8 = 3, R9 = 4
    ; [rbp + 16] = 5, [rbp + 24] = 6, [rbp + 32] = 7
    
    mov rax, rcx                   ; Start with 1st parameter
    add rax, rdx                   ; Add 2nd
    add rax, r8                    ; Add 3rd  
    add rax, r9                    ; Add 4th
    add rax, qword ptr [rbp + 16]  ; Add 5th
    add rax, qword ptr [rbp + 24]  ; Add 6th
    add rax, qword ptr [rbp + 32]  ; Add 7th
    
    pop rbp
    ret
many_param_func ENDP
```

### Register Preservation
Certain registers must be preserved across function calls:

#### Volatile Registers (Caller-saved)
```assembly
; These registers may be modified by called functions:
; RAX, RCX, RDX, R8, R9, R10, R11
; XMM0-XMM5

; Caller must save them if needed after call
push r10                           ; Save volatile register
push r11
call some_function                 ; May modify R10, R11
pop r11                            ; Restore
pop r10
```

#### Non-volatile Registers (Callee-saved)
```assembly
; These registers must be preserved by called functions:
; RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15
; XMM6-XMM15

my_function PROC
    push rbx                       ; Save non-volatile register
    push r12
    push r13
    
    ; Use RBX, R12, R13 for computation
    mov rbx, rcx                   ; Use RBX
    mov r12, rdx                   ; Use R12
    
    ; Function body using non-volatile registers
    
    pop r13                        ; Restore in reverse order
    pop r12
    pop rbx
    ret
my_function ENDP
```

### Recursive Function Implementation

#### Factorial Example
```assembly
; Recursive factorial function
factorial PROC
    push rbp
    mov rbp, rsp
    sub rsp, 32                    ; Shadow space
    
    ; Base case: if n <= 1, return 1
    cmp rcx, 1
    jle base_case
    
    ; Recursive case: n * factorial(n-1)
    push rcx                       ; Save n
    dec rcx                        ; n - 1
    call factorial                 ; factorial(n-1)
    pop rcx                        ; Restore n
    imul rax, rcx                  ; n * factorial(n-1)
    jmp epilogue
    
base_case:
    mov rax, 1                     ; Return 1
    
epilogue:
    add rsp, 32
    pop rbp
    ret
factorial ENDP
```

### Stack-Based Local Variables
```assembly
my_function PROC
    push rbp
    mov rbp, rsp
    sub rsp, 64                    ; 32 bytes shadow + 32 bytes locals
    
    ; Local variables (relative to RBP):
    ; [rbp - 8]  = local1 (qword)
    ; [rbp - 16] = local2 (qword) 
    ; [rbp - 24] = local3 (qword)
    ; [rbp - 32] = local4 (qword)
    
    mov qword ptr [rbp - 8], 100   ; local1 = 100
    mov qword ptr [rbp - 16], 200  ; local2 = 200
    
    ; Use local variables
    mov rax, qword ptr [rbp - 8]   ; Load local1
    add rax, qword ptr [rbp - 16]  ; Add local2
    mov qword ptr [rbp - 24], rax  ; Store in local3
    
    add rsp, 64
    pop rbp
    ret
my_function ENDP
```

### Exception Handling Considerations
Windows requires proper stack unwinding for exception handling:

```assembly
; Functions that may throw exceptions should maintain proper stack frames
safe_function PROC
    push rbp                       ; Required for unwinding
    mov rbp, rsp                   ; Frame pointer for unwinding
    sub rsp, 32                    ; Shadow space
    
    ; Code that might cause exception
    mov rax, qword ptr [rcx]       ; Potential access violation
    
    ; Normal return path
    add rsp, 32
    pop rbp
    ret
safe_function ENDP
```

### Advanced: Function Pointers and Callbacks
```assembly
; Function pointer type
CALLBACK_FUNC typedef PROTO :QWORD

; Function that takes callback
process_with_callback PROC
    ; RCX = callback function pointer
    ; RDX = data to process
    
    push rbp
    mov rbp, rsp
    sub rsp, 32
    
    ; Call the callback function
    push rcx                       ; Save callback pointer
    mov rcx, rdx                   ; Pass data as parameter
    call qword ptr [rbp - 8]       ; Call callback
    
    add rsp, 32
    pop rbp
    ret
process_with_callback ENDP

; Callback implementation
my_callback PROC
    ; RCX = data parameter
    add rcx, rcx                   ; Double the value
    mov rax, rcx                   ; Return doubled value
    ret
my_callback ENDP

; Usage:
main PROC
    mov rcx, offset my_callback    ; Function pointer
    mov rdx, 21                    ; Data to process
    call process_with_callback     ; Result in RAX = 42
    ret
main ENDP
```

## Problem
Implement recursive function with proper Windows x64 calling convention.

### Skills Covered
- Windows x64 calling convention
- Stack frame management
- Recursive function calls
- Parameter passing and return values

### Your Task
Implement Fibonacci function using proper calling convention:

**tutorial4.asm**:
```assembly
; Tutorial 4: Function calls and stack manipulation
.code

ExitProcess PROTO
printf PROTO

; TODO: Implement fibonacci function
; fibonacci PROC
;     ; RCX = n (input parameter)
;     ; RAX = result (return value)
;     ; Implement: fib(0)=0, fib(1)=1, fib(n)=fib(n-1)+fib(n-2)
; fibonacci ENDP

.data
    test_values dq 0, 1, 5, 10, 15
    test_count equ ($ - test_values) / 8
    result_msg db 'fibonacci(%lld) = %lld', 0Dh, 0Ah, 0

.code
main PROC
    sub rsp, 32                    ; Shadow space
    
    ; Test fibonacci function with different values
    mov rbx, 0                     ; Index counter
    
test_loop:
    cmp rbx, test_count
    jge done
    
    ; Get test value and call fibonacci
    mov rcx, qword ptr [test_values + rbx*8]
    push rbx                       ; Save index
    call fibonacci                 ; Your implementation
    pop rbx                        ; Restore index
    
    ; Print result
    push rax                       ; Save result
    push rbx                       ; Save index
    mov r8, rax                    ; fibonacci result
    mov rdx, qword ptr [test_values + rbx*8]  ; input value
    lea rcx, result_msg            ; format string
    call printf
    pop rbx                        ; Restore index
    pop rax                        ; Restore result
    
    inc rbx                        ; Next test
    jmp test_loop
    
done:
    add rsp, 32                    ; Clean up shadow space
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Expected Output:** Fibonacci sequence calculations with proper recursion

---

# Tutorial 5: Debugging C++ Libraries with Visual Studio and WinDbg

## Lesson: Debugging Windows Applications at Assembly Level

### Visual Studio Debugger Setup
When debugging C++ libraries without source code, Visual Studio provides powerful assembly-level debugging:

#### Enabling Disassembly View
```
1. Debug → Windows → Disassembly (or Alt+8)
2. Tools → Options → Debugging → General → "Show disassembly if source is not available"
3. Debug → Windows → Registers (to view CPU registers)
4. Debug → Windows → Memory (to examine memory contents)
```

#### Debugging Commands in Visual Studio
```
F10 - Step Over (next instruction)
F11 - Step Into (follow calls)  
Shift+F11 - Step Out (return from function)
F5 - Continue execution
F9 - Toggle breakpoint
Ctrl+Alt+D - Disassembly window
Ctrl+Alt+R - Registers window
```

### WinDbg Setup and Commands
WinDbg is Microsoft's powerful command-line debugger:

#### Basic WinDbg Commands
```
.load mscordacwks              # Load .NET debugging extensions
bp address                     # Set breakpoint at address
bp module!function             # Set breakpoint at function
bl                             # List breakpoints
g                              # Go (continue execution)
t                              # Trace (step into)
p                              # Step over
k                              # Stack trace
r                              # Show registers
d address                      # Dump memory
u address                      # Unassemble code
lm                             # List modules
x module!*                     # List symbols in module
```

### Debugging Stripped C++ Library

#### Setting Up the Environment
**mystery_lib.h** (interface only):
```cpp
// Public interface - implementation hidden
extern "C" {
    __declspec(dllexport) int process_data(int input);
    __declspec(dllexport) void* create_object(int size);
    __declspec(dllexport) int call_method(void* obj, int value);
    __declspec(dllexport) void destroy_object(void* obj);
}
```

**tutorial5.cpp** (test application):
```cpp
#include <iostream>
#include <windows.h>

// Function pointers for dynamic loading
typedef int (*ProcessDataFunc)(int);
typedef void* (*CreateObjectFunc)(int);
typedef int (*CallMethodFunc)(void*, int);
typedef void (*DestroyObjectFunc)(void*);

int main() {
    // Load the mystery library
    HMODULE hLib = LoadLibrary(L"mystery_lib.dll");
    if (!hLib) {
        std::cerr << "Failed to load library\n";
        return 1;
    }
    
    // Get function pointers
    ProcessDataFunc processData = (ProcessDataFunc)GetProcAddress(hLib, "process_data");
    CreateObjectFunc createObj = (CreateObjectFunc)GetProcAddress(hLib, "create_object");
    CallMethodFunc callMethod = (CallMethodFunc)GetProcAddress(hLib, "call_method");
    DestroyObjectFunc destroyObj = (DestroyObjectFunc)GetProcAddress(hLib, "destroy_object");
    
    if (!processData || !createObj || !callMethod || !destroyObj) {
        std::cerr << "Failed to get function addresses\n";
        FreeLibrary(hLib);
        return 1;
    }
    
    // Test the functions - set breakpoints here for debugging
    int result1 = processData(42);
    std::cout << "process_data(42) = " << result1 << std::endl;
    
    void* obj = createObj(100);
    int result2 = callMethod(obj, 25);
    std::cout << "call_method(obj, 25) = " << result2 << std::endl;
    
    destroyObj(obj);
    FreeLibrary(hLib);
    
    return 0;
}
```

### Visual Studio Debugging Session

#### Step 1: Start Debugging
```
1. Set breakpoint on processData(42) call
2. Press F5 to start debugging
3. When breakpoint hits, open Disassembly window
4. You'll see something like:

00007FF6`12341000 48894C2408      mov qword ptr [rsp+8], rcx
00007FF6`12341005 4883EC28        sub rsp, 28h
00007FF6`12341009 488B4C2430      mov rcx, qword ptr [rsp+30h]
00007FF6`1234100E E8CDABFFFF      call mystery_lib!process_data (00007FF6`1233BBE0)
```

#### Step 2: Examine Function Entry
```
1. Press F11 to step into process_data
2. Observe the disassembly:

mystery_lib!process_data:
00007FF8`9ABC1000 48894C2408      mov qword ptr [rsp+8], rcx    ; Save parameter
00007FF8`9ABC1005 4883EC28        sub rsp, 28h                  ; Allocate stack space
00007FF8`9ABC1009 8B442430        mov eax, dword ptr [rsp+30h]  ; Load parameter into EAX
00007FF8`9ABC100D 83C00A          add eax, 0Ah                  ; Add 10 to parameter
00007FF8`9ABC1010 4883C428        add rsp, 28h                  ; Deallocate stack
00007FF8`9ABC1014 C3              ret                           ; Return
```

#### Step 3: Analyze the Pattern
From the disassembly, we can deduce:
- Function adds 10 to its input parameter
- Uses standard Windows x64 calling convention
- Simple arithmetic operation

### WinDbg Debugging Session

#### Attach to Process
```cmd
# Start WinDbg and attach to running process
windbg -pn tutorial5.exe

# Or start process under debugger
windbg tutorial5.exe
```

#### Setting Breakpoints
```
# Set breakpoint at main function
bp tutorial5!main

# Set breakpoint at library function (if symbols available)
bp mystery_lib!process_data

# Set breakpoint by address (if no symbols)
bp 0x00007ff89abc1000

# Run the program
g
```

#### Examining Memory and Registers
```
# When breakpoint hits, examine registers
r

# Show stack
k

# Disassemble current location
u @rip L20

# Examine memory at specific address
db 0x00007ff89abc1000 L100

# Show loaded modules
lm

# Find exported functions
x mystery_lib!*
```

### Advanced Debugging Techniques

#### Dynamic Analysis with API Monitor
```cpp
// API Monitor can intercept and log function calls
// Shows parameters and return values without source code

// Example output:
// mystery_lib.dll  process_data(42) -> 52
// mystery_lib.dll  create_object(100) -> 0x00000123456789AB
// mystery_lib.dll  call_method(0x00000123456789AB, 25) -> 625
```

#### Using Performance Toolkit
```cmd
# Use Windows Performance Toolkit for tracing
wpa                           # Windows Performance Analyzer
xperf                         # Command-line tracing tool

# Example tracing session
xperf -on PROC_THREAD+LOADER+CSWITCH -f trace.etl
# Run your application
xperf -d trace.etl
wpa trace.etl                 # Analyze trace
```

#### Memory Analysis with Application Verifier
```cmd
# Enable Application Verifier for heap debugging
appverif                      # GUI tool
# Select your application and enable heap verification

# Run application - verifier will break on heap issues
```

### Reverse Engineering Assembly Patterns

#### Identifying C++ Constructors
```assembly
; C++ constructor pattern
mystery_lib!Constructor:
00007FF8`9ABC2000 48894C2408      mov qword ptr [rsp+8], rcx    ; 'this' pointer
00007FF8`9ABC2005 4883EC28        sub rsp, 28h
00007FF8`9ABC2009 488B442430      mov rcx, qword ptr [rsp+30h]  ; Load 'this'
00007FF8`9ABC200E 48C70100000000  mov qword ptr [rcx], offset vtable  ; Set vtable
00007FF8`9ABC2015 488B442430      mov rcx, qword ptr [rsp+30h]  ; Return 'this'
00007FF8`9ABC201A 4883C428        add rsp, 28h
00007FF8`9ABC201E C3              ret
```

#### Identifying Virtual Function Calls
```assembly
; Virtual function call pattern
00007FF8`9ABC3000 488B01          mov rax, qword ptr [rcx]      ; Load vtable
00007FF8`9ABC3003 FF5008          call qword ptr [rax+8]        ; Call vtable[1]
```

#### Identifying Exception Handling
```assembly
; SEH (Structured Exception Handling) pattern
00007FF8`9ABC4000 4881EC00010000  sub rsp, 100h                 ; Large stack allocation
00007FF8`9ABC4007 488D0500000000  lea rax, [exception_handler]
00007FF8`9ABC400E 4889442420      mov qword ptr [rsp+20h], rax  ; Set exception handler
```

### Creating Debug Scripts

#### Visual Studio Debug Script
```xml
<!-- Save as debug_mystery.natvis -->
<?xml version="1.0" encoding="utf-8"?>
<AutoVisualizer xmlns="http://schemas.microsoft.com/vstudio/debugger/natvis/2010">
  <Type Name="MysteryObject">
    <DisplayString>MysteryObject at {this}</DisplayString>
    <Expand>
      <Item Name="[vtable]">*(void**)this</Item>
      <Item Name="[data]">*(int*)((char*)this + 8)</Item>
    </Expand>
  </Type>
</AutoVisualizer>
```

#### WinDbg Script
```javascript
// Save as debug_mystery.js
function processBreakpoint() {
    host.diagnostics.debugLog("Breaking at process_data\n");
    host.diagnostics.debugLog("Parameter: " + host.currentThread.Registers.User.rcx + "\n");
    
    // Continue execution
    return false;
}

function initializeScript() {
    return [new host.functionAlias(processBreakpoint, "bp_process")];
}
```

## Problem
Debug a mystery C++ library using Visual Studio and assembly analysis.

### Skills Covered
- Visual Studio assembly debugging
- WinDbg command-line debugging
- Pattern recognition in compiled code
- Dynamic library analysis

### Your Task
Analyze the provided mystery library:

**tutorial5_debug.cpp**:
```cpp
// Tutorial 5: Debug the mystery library
#include <iostream>
#include <windows.h>

// TODO: Load mystery_lib.dll and analyze these functions:
// 1. int mystery_function1(int x) - determine what it computes
// 2. void* mystery_function2(int size) - understand memory allocation
// 3. int mystery_function3(void* ptr, int value) - analyze operation
// 4. void mystery_function4(void* ptr) - understand cleanup

int main() {
    // TODO: Load library and test functions
    // TODO: Use debugger to analyze assembly code
    // TODO: Document your findings
    
    HMODULE hLib = LoadLibrary(L"mystery_lib.dll");
    if (!hLib) {
        std::cerr << "Library not found - create stub version for testing\n";
        return 1;
    }
    
    // Get function pointers and test
    // Set breakpoints and analyze disassembly
    
    FreeLibrary(hLib);
    return 0;
}
```

**Expected Analysis:**
- Function behavior documentation
- Assembly pattern recognition  
- Memory layout understanding
- Performance characteristics

---

# Tutorial 6: String Operations and Memory Manipulation

## Lesson: x64 String Instructions and Windows Memory Management

### Windows String Handling
Windows provides both ASCII and Unicode string functions. MASM supports efficient string operations using x64 string instructions:

```assembly
; Windows string conventions:
; LPSTR = char* (ASCII)
; LPWSTR = wchar_t* (Unicode/UTF-16)
; LPTSTR = TCHAR* (ASCII or Unicode depending on build)
```

### x64 String Instructions
Modern x64 processors provide optimized string instructions:

#### Direction Flag Control
```assembly
cld                           ; Clear direction flag (forward processing)
std                           ; Set direction flag (backward processing)
```

#### String Scan (SCAS)
```assembly
; Search for character in string
scasb                         ; Compare AL with [RDI], then RDI++
scasw                         ; Compare AX with [RDI], then RDI += 2
scasd                         ; Compare EAX with [RDI], then RDI += 4
scasq                         ; Compare RAX with [RDI], then RDI += 8
```

#### String Load and Store (LODS/STOS)
```assembly
; Load from string
lodsb                         ; AL = [RSI], then RSI++
lodsw                         ; AX = [RSI], then RSI += 2
lodsd                         ; EAX = [RSI], then RSI += 4
lodsq                         ; RAX = [RSI], then RSI += 8

; Store to string
stosb                         ; [RDI] = AL, then RDI++  
stosw                         ; [RDI] = AX, then RDI += 2
stosd                         ; [RDI] = EAX, then RDI += 4
stosq                         ; [RDI] = RAX, then RDI += 8
```

#### String Compare (CMPS)
```assembly
; Compare strings
cmpsb                         ; Compare [RSI] with [RDI], then RSI++, RDI++
cmpsw                         ; Compare [RSI] with [RDI], then RSI += 2, RDI += 2
cmpsd                         ; Compare [RSI] with [RDI], then RSI += 4, RDI += 4
cmpsq                         ; Compare [RSI] with [RDI], then RSI += 8, RDI += 8  
```

### REP Prefixes for Automation
REP prefixes allow automatic repetition of string instructions:

```assembly
rep   instruction             ; Repeat RCX times
repe  instruction             ; Repeat while equal and RCX > 0
repne instruction             ; Repeat while not equal and RCX > 0
```

### Practical String Functions

#### String Length Implementation
```assembly
; Custom strlen function
my_strlen PROC
    ; Input: RCX = string pointer
    ; Output: RAX = string length
    
    mov rdi, rcx                  ; String pointer to RDI
    mov al, 0                     ; Search for null terminator
    mov rcx, -1                   ; Maximum possible length
    cld                           ; Forward direction
    repne scasb                   ; Scan until AL found or RCX = 0
    not rcx                       ; Complement RCX
    dec rcx                       ; Don't count null terminator
    mov rax, rcx                  ; Return length
    ret
my_strlen ENDP
```

#### String Copy Implementation  
```assembly
; Custom strcpy function
my_strcpy PROC
    ; Input: RCX = destination, RDX = source
    ; Output: RAX = destination
    
    push rdi
    push rsi
    
    mov rdi, rcx                  ; Destination
    mov rsi, rdx                  ; Source
    mov rax, rcx                  ; Save destination for return
    cld                           ; Forward direction
    
copy_loop:
    lodsb                         ; Load byte from source
    stosb                         ; Store byte to destination
    test al, al                   ; Check for null terminator
    jnz copy_loop                 ; Continue if not null
    
    pop rsi
    pop rdi
    ret
my_strcpy ENDP
```

#### String Compare Implementation
```assembly
; Custom strcmp function  
my_strcmp PROC
    ; Input: RCX = string1, RDX = string2
    ; Output: RAX = 0 (equal), <0 (str1 < str2), >0 (str1 > str2)
    
    push rdi
    push rsi
    
    mov rsi, rcx                  ; String1
    mov rdi, rdx                  ; String2
    cld                           ; Forward direction
    
compare_loop:
    lodsb                         ; Load byte from string1
    mov ah, byte ptr [rdi]        ; Load byte from string2
    inc rdi                       ; Move to next character
    
    cmp al, ah                    ; Compare characters
    jne not_equal                 ; If different, determine result
    
    test al, al                   ; Check if end of string
    jnz compare_loop              ; Continue if not end
    
    ; Strings are equal
    xor rax, rax                  ; Return 0
    jmp compare_done
    
not_equal:
    movzx rax, al                 ; Zero-extend AL to RAX
    movzx rcx, ah                 ; Zero-extend AH to RCX  
    sub rax, rcx                  ; RAX = char1 - char2
    
compare_done:
    pop rsi
    pop rdi
    ret
my_strcmp ENDP
```

### Memory Block Operations

#### Memory Set (Fast Fill)
```assembly
; Fast memory fill using REP STOS
fast_memset PROC
    ; Input: RCX = buffer, RDX = value, R8 = count
    ; Output: RAX = buffer
    
    push rdi
    
    mov rdi, rcx                  ; Buffer pointer
    mov rax, rdx                  ; Fill value
    mov rcx, r8                   ; Count
    mov rdx, rcx                  ; Save original buffer
    
    ; Expand byte value to qword for efficiency
    mov ah, al                    ; AX = value repeated
    mov dx, ax                    ; DX = value repeated
    shl rax, 16
    mov ax, dx                    ; EAX = value repeated 4 times
    mov rdx, rax
    shl rax, 32
    or rax, rdx                   ; RAX = value repeated 8 times
    
    ; Fill 8 bytes at a time
    mov rdx, rcx
    shr rcx, 3                    ; Count / 8
    rep stosq                     ; Fill qwords
    
    ; Fill remaining bytes
    mov rcx, rdx
    and rcx, 7                    ; Count % 8
    rep stosb                     ; Fill remaining bytes
    
    mov rax, rdi                  ; Return buffer
    pop rdi
    ret
fast_memset ENDP
```

#### Memory Copy (Optimized)
```assembly
; Optimized memory copy
fast_memcpy PROC  
    ; Input: RCX = dest, RDX = src, R8 = count
    ; Output: RAX = dest
    
    push rdi
    push rsi
    
    mov rdi, rcx                  ; Destination
    mov rsi, rdx                  ; Source
    mov rax, rcx                  ; Save destination
    mov rcx, r8                   ; Count
    
    ; Copy 8 bytes at a time for efficiency
    mov rdx, rcx
    shr rcx, 3                    ; Count / 8
    cld                           ; Forward direction
    rep movsq                     ; Copy qwords
    
    ; Copy remaining bytes
    mov rcx, rdx
    and rcx, 7                    ; Count % 8
    rep movsb                     ; Copy remaining bytes
    
    pop rsi
    pop rdi
    ret
fast_memcpy ENDP
```

### Windows API String Functions
Integration with Windows string APIs:

```assembly
; Using Windows API functions
.data
    test_str db 'Hello, World!', 0
    wide_str dw 'H','e','l','l','o', 0
    buffer db 100 dup(0)

.code
string_api_demo PROC
    ; Get string length using lstrlenA
    lea rcx, test_str
    call lstrlenA                 ; Windows API function
    ; RAX now contains string length
    
    ; Copy string using lstrcpyA  
    lea rcx, buffer               ; Destination
    lea rdx, test_str             ; Source
    call lstrcpyA                 ; Windows API function
    
    ; Compare strings using lstrcmpA
    lea rcx, buffer               ; String 1
    lea rdx, test_str             ; String 2  
    call lstrcmpA                 ; Returns 0 if equal
    
    ret
string_api_demo ENDP
```

### Unicode String Handling
Windows extensively uses Unicode (UTF-16):

```assembly
; Unicode string operations
.data
    unicode_str dw 'H','e','l','l','o',',',' ','W','o','r','l','d','!', 0
    unicode_buffer dw 100 dup(0)

.code
unicode_strlen PROC
    ; Input: RCX = Unicode string pointer
    ; Output: RAX = character count (not byte count)
    
    mov rdi, rcx                  ; String pointer
    mov ax, 0                     ; Search for null terminator (word)
    mov rcx, -1                   ; Maximum length
    cld                           ; Forward direction
    repne scasw                   ; Scan words until AX found
    not rcx                       ; Complement RCX
    dec rcx                       ; Don't count null terminator
    mov rax, rcx                  ; Return character count
    ret
unicode_strlen ENDP

unicode_strcpy PROC
    ; Input: RCX = destination, RDX = source
    ; Output: RAX = destination
    
    push rdi
    push rsi
    
    mov rdi, rcx                  ; Destination
    mov rsi, rdx                  ; Source
    mov rax, rcx                  ; Save destination
    cld                           ; Forward direction
    
unicode_copy_loop:
    lodsw                         ; Load word from source
    stosw                         ; Store word to destination  
    test ax, ax                   ; Check for null terminator
    jnz unicode_copy_loop         ; Continue if not null
    
    pop rsi
    pop rdi
    ret
unicode_strcpy ENDP
```

### Performance Optimization Techniques

#### Cache-Friendly String Operations
```assembly
; Process strings in cache-line friendly chunks
optimized_string_process PROC
    ; Process 64-byte chunks (typical cache line size)
    mov rcx, string_length
    shr rcx, 6                    ; Divide by 64
    
cache_line_loop:
    ; Process 64 bytes of string data
    ; ... processing code ...
    add rsi, 64                   ; Next cache line
    dec rcx
    jnz cache_line_loop
    
    ; Handle remaining bytes
    mov rcx, string_length
    and rcx, 63                   ; Length % 64
    ; ... handle remainder ...
    
    ret
optimized_string_process ENDP
```

#### SIMD String Operations (Preview)
```assembly
; Using SSE for faster string operations (covered in Tutorial 9)
sse_memcmp PROC
    ; Compare 16 bytes at a time using SSE
    movdqu xmm0, xmmword ptr [rsi]    ; Load 16 bytes from string1
    movdqu xmm1, xmmword ptr [rdi]    ; Load 16 bytes from string2
    pcmpeqb xmm0, xmm1                ; Compare bytes
    pmovmskb eax, xmm0                ; Create mask from comparison
    cmp eax, 0xFFFF                   ; All bytes equal?
    jne strings_differ                ; Jump if difference found
    
    ; Continue processing...
    ret
sse_memcmp ENDP
```

## Problem
Implement efficient string operations using x64 string instructions.

### Skills Covered
- String scanning with `scasb`
- String copying with `lodsb`/`stosb`
- String comparison with `cmpsb`
- Memory manipulation with REP prefixes

### Your Task
Create comprehensive string processing library:

**tutorial6.asm**:
```assembly
; Tutorial 6: String operations and memory manipulation
.code

ExitProcess PROTO
printf PROTO
lstrlenA PROTO
lstrcpyA PROTO
lstrcmpA PROTO

; TODO: Implement these string functions
; my_strlen PROC
; my_strcpy PROC  
; my_strcat PROC
; my_strcmp PROC
; my_memset PROC
; my_memcpy PROC

.data
    test_str1 db 'Programming', 0
    test_str2 db 'Assembly', 0
    buffer1 db 50 dup(0)
    buffer2 db 50 dup(0)
    
    test_results db 'strlen: %lld, strcmp: %d', 0Dh, 0Ah, 0
    copy_result db 'Copied: %s', 0Dh, 0Ah, 0
    concat_result db 'Concatenated: %s', 0Dh, 0Ah, 0

.code
main PROC
    sub rsp, 32                       ; Shadow space
    
    ; TODO: Test your string functions
    ; 1. Calculate length of test_str1
    ; 2. Copy test_str1 to buffer1
    ; 3. Concatenate test_str2 to buffer1  
    ; 4. Compare original strings
    ; 5. Demonstrate memory operations
    
    ; Example test framework:
    lea rcx, test_str1
    call my_strlen                    ; Your implementation
    mov rbx, rax                      ; Save length
    
    ; Test strcpy
    lea rcx, buffer1                  ; Destination
    lea rdx, test_str1                ; Source
    call my_strcpy                    ; Your implementation
    
    ; Print results
    lea rcx, copy_result
    lea rdx, buffer1
    call printf
    
    ; Additional tests...
    
    add rsp, 32
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Expected Output:** String processing results demonstrating efficient operations

---

# Tutorial 7: Array Processing and Pointer Arithmetic

## Lesson: Memory Addressing and Array Manipulation

### Understanding Array Memory Layout
Arrays in memory are contiguous blocks of elements:

```assembly
; Array declarations in MASM
.data
    byte_array    db 10, 20, 30, 40, 50          ; 5 bytes
    word_array    dw 100, 200, 300, 400          ; 4 words (8 bytes)
    dword_array   dd 1000, 2000, 3000            ; 3 dwords (12 bytes)
    qword_array   dq 10000, 20000, 30000, 40000  ; 4 qwords (32 bytes)
    
    ; Multi-dimensional array (2x3 matrix)
    matrix        dd 1, 2, 3, 4, 5, 6            ; Row-major order
```

### Memory Layout Visualization
```
byte_array:   [10][20][30][40][50]
              ^   ^   ^   ^   ^
Addresses:    +0  +1  +2  +3  +4

qword_array:  [10000    ][20000    ][30000    ][40000    ]
              ^          ^          ^          ^
Addresses:    +0         +8         +16        +24

matrix (2x3): [1 ][2 ][3 ][4 ][5 ][6 ]
              ^   ^   ^   ^   ^   ^
              Row 0     Row 1
```

### Addressing Modes for Arrays
x64 provides powerful addressing modes perfect for array access:

```assembly
; Basic array indexing patterns
mov rax, qword ptr [array_base + index*8]      ; array[index] for qwords
mov eax, dword ptr [array_base + index*4]      ; array[index] for dwords
mov ax, word ptr [array_base + index*2]        ; array[index] for words
mov al, byte ptr [array_base + index]          ; array[index] for bytes

; Using index register
mov rsi, offset array_base                      ; Base address
mov rcx, 3                                      ; Index
mov rax, qword ptr [rsi + rcx*8]               ; Load array[3]
```

### Advanced Addressing Examples

#### Multi-dimensional Array Access
```assembly
; Accessing matrix[row][col] where matrix is MxN
; Address = base + (row * N + col) * sizeof(element)

.data
    matrix dd 1, 2, 3, 4, 5, 6, 7, 8, 9         ; 3x3 matrix
    ROWS equ 3
    COLS equ 3

.code
get_matrix_element PROC
    ; Input: RCX = row, RDX = col
    ; Output: RAX = matrix[row][col]
    
    mov rax, rcx                                 ; RAX = row
    imul rax, COLS                               ; RAX = row * COLS
    add rax, rdx                                 ; RAX = row * COLS + col
    mov eax, dword ptr [matrix + rax*4]          ; Load matrix[row][col]
    ret
get_matrix_element ENDP
```

#### Pointer Arithmetic
```assembly
; Walking through array with pointers
array_sum PROC
    ; Input: RCX = array pointer, RDX = count
    ; Output: RAX = sum of elements
    
    xor rax, rax                                 ; Sum = 0
    test rdx, rdx                                ; Check if count is 0
    jz sum_done
    
sum_loop:
    add rax, qword ptr [rcx]                     ; Add current element
    add rcx, 8                                   ; Move to next element (8 bytes)
    dec rdx                                      ; Decrement count
    jnz sum_loop                                 ; Continue if more elements
    
sum_done:
    ret
array_sum ENDP
```

### Array Search Algorithms

#### Linear Search
```assembly
linear_search PROC
    ; Input: RCX = array, RDX = count, R8 = search value
    ; Output: RAX = index (or -1 if not found)
    
    xor rax, rax                                 ; Index = 0
    test rdx, rdx                                ; Check if count is 0
    jz not_found
    
search_loop:
    cmp qword ptr [rcx + rax*8], r8              ; Compare array[index] with value
    je found                                     ; Jump if found
    inc rax                                      ; Next index
    cmp rax, rdx                                 ; Check bounds
    jl search_loop                               ; Continue if within bounds
    
not_found:
    mov rax, -1                                  ; Return -1 (not found)
    
found:
    ret
linear_search ENDP
```

#### Binary Search (Sorted Array)
```assembly
binary_search PROC
    ; Input: RCX = sorted array, RDX = count, R8 = search value
    ; Output: RAX = index (or -1 if not found)
    
    xor r9, r9                                   ; Low = 0
    mov r10, rdx                                 ; High = count
    dec r10                                      ; High = count - 1
    
binary_loop:
    cmp r9, r10                                  ; Compare low with high
    jg not_found_binary                          ; If low > high, not found
    
    mov rax, r9                                  ; RAX = low
    add rax, r10                                 ; RAX = low + high
    shr rax, 1                                   ; RAX = (low + high) / 2 (mid)
    
    mov r11, qword ptr [rcx + rax*8]             ; R11 = array[mid]
    cmp r11, r8                                  ; Compare array[mid] with value
    je found_binary                              ; Found it!
    jl search_upper                              ; If array[mid] < value, search upper half
    
    ; Search lower half
    dec rax                                      ; mid - 1
    mov r10, rax                                 ; High = mid - 1
    jmp binary_loop
    
search_upper:
    inc rax                                      ; mid + 1  
    mov r9, rax                                  ; Low = mid + 1
    jmp binary_loop
    
not_found_binary:
    mov rax, -1                                  ; Return -1
    
found_binary:
    ret
binary_search ENDP
```

### Sorting Algorithms

#### Bubble Sort Implementation
```assembly
bubble_sort PROC
    ; Input: RCX = array, RDX = count
    ; Sorts array in ascending order
    
    push rbx
    push r12
    push r13
    
    mov r12, rcx                                 ; Array pointer
    mov r13, rdx                                 ; Count
    
    ; Outer loop: i = 0; i < count-1; i++
    xor rbx, rbx                                 ; i = 0
    
outer_loop:
    mov rax, r13                                 ; count
    dec rax                                      ; count - 1
    cmp rbx, rax                                 ; i < count - 1?
    jge sort_done                                ; Exit if i >= count - 1
    
    ; Inner loop: j = 0; j < count-1-i; j++
    xor rcx, rcx                                 ; j = 0
    
inner_loop:
    mov rax, r13                                 ; count
    dec rax                                      ; count - 1
    sub rax, rbx                                 ; count - 1 - i
    cmp rcx, rax                                 ; j < count - 1 - i?
    jge next_outer                               ; Exit inner loop
    
    ; Compare array[j] and array[j+1]
    mov rax, qword ptr [r12 + rcx*8]             ; array[j]
    mov rdx, rcx                                 ; j
    inc rdx                                      ; j + 1
    mov r8, qword ptr [r12 + rdx*8]              ; array[j+1]
    
    cmp rax, r8                                  ; array[j] > array[j+1]?
    jle no_swap                                  ; Skip if in order
    
    ; Swap elements
    mov qword ptr [r12 + rcx*8], r8              ; array[j] = array[j+1]
    mov qword ptr [r12 + rdx*8], rax             ; array[j+1] = array[j]
    
no_swap:
    inc rcx                                      ; j++
    jmp inner_loop
    
next_outer:
    inc rbx                                      ; i++
    jmp outer_loop
    
sort_done:
    pop r13
    pop r12
    pop rbx
    ret
bubble_sort ENDP
```

### Array Processing Patterns

#### Finding Maximum Element
```assembly
find_max PROC
    ; Input: RCX = array, RDX = count
    ; Output: RAX = maximum value, RBX = index of maximum
    
    test rdx, rdx                                ; Check if count is 0
    jz empty_array
    
    mov rax, qword ptr [rcx]                     ; Max = first element
    xor rbx, rbx                                 ; Max index = 0
    mov r8, 1                                    ; Current index = 1
    
max_loop:
    cmp r8, rdx                                  ; Current index < count?
    jge max_done                                 ; Exit if done
    
    mov r9, qword ptr [rcx + r8*8]               ; Current element
    cmp r9, rax                                  ; Current > max?
    jle max_continue                             ; Skip if not greater
    
    mov rax, r9                                  ; Update max value
    mov rbx, r8                                  ; Update max index
    
max_continue:
    inc r8                                       ; Next index
    jmp max_loop
    
empty_array:
    xor rax, rax                                 ; Return 0 for empty array
    xor rbx, rbx
    
max_done:
    ret
find_max ENDP
```

#### Array Reversal
```assembly
reverse_array PROC
    ; Input: RCX = array, RDX = count
    ; Reverses array in place
    
    test rdx, rdx                                ; Check if count is 0
    jz reverse_done
    
    xor r8, r8                                   ; Left index = 0
    mov r9, rdx                                  ; Right index = count
    dec r9                                       ; Right index = count - 1
    
reverse_loop:
    cmp r8, r9                                   ; Left >= right?
    jge reverse_done                             ; Exit if pointers meet
    
    ; Swap elements at left and right indices
    mov rax, qword ptr [rcx + r8*8]              ; Left element
    mov rbx, qword ptr [rcx + r9*8]              ; Right element
    mov qword ptr [rcx + r8*8], rbx              ; Store right at left
    mov qword ptr [rcx + r9*8], rax              ; Store left at right
    
    inc r8                                       ; Move left pointer right
    dec r9                                       ; Move right pointer left
    jmp reverse_loop
    
reverse_done:
    ret
reverse_array ENDP
```

### Memory-Efficient Array Operations

#### In-Place Array Rotation
```assembly
rotate_array PROC
    ; Input: RCX = array, RDX = count, R8 = positions to rotate left
    ; Rotates array left by R8 positions
    
    test rdx, rdx                                ; Check if count is 0
    jz rotate_done
    
    ; Normalize rotation count
    xor rax, rax
    mov rax, r8                                  ; Rotation count
    xor rdx, rdx                                 ; Clear RDX for division
    div rdx                                      ; R8 = R8 % count
    mov r8, rdx                                  ; Use remainder
    
    test r8, r8                                  ; Check if rotation is 0
    jz rotate_done
    
    ; Implement array rotation using reversal algorithm:
    ; 1. Reverse first R8 elements
    ; 2. Reverse remaining elements  
    ; 3. Reverse entire array
    
    ; Step 1: Reverse first R8 elements
    push rcx
    push rdx
    mov rdx, r8                                  ; Count = rotation positions
    call reverse_array
    pop rdx
    pop rcx
    
    ; Step 2: Reverse remaining elements
    push rcx
    lea rcx, [rcx + r8*8]                        ; Start after rotated portion
    sub rdx, r8                                  ; Count = remaining elements
    call reverse_array
    pop rcx
    
    ; Step 3: Reverse entire array
    ; RCX and original RDX are already set
    call reverse_array
    
rotate_done:
    ret
rotate_array ENDP
```

### SIMD Array Processing Preview
```assembly
; Preview of SIMD operations (detailed in Tutorial 9)
sse_array_add PROC
    ; Add corresponding elements of two arrays using SSE
    ; Input: RCX = array1, RDX = array2, R8 = result, R9 = count (must be multiple of 2)
    
    shr r9, 1                                    ; Process 2 elements at a time
    
sse_loop:
    movdqu xmm0, xmmword ptr [rcx]               ; Load 2 qwords from array1
    movdqu xmm1, xmmword ptr [rdx]               ; Load 2 qwords from array2
    paddq xmm0, xmm1                             ; Add corresponding elements
    movdqu xmmword ptr [r8], xmm0                ; Store result
    
    add rcx, 16                                  ; Next 2 elements in array1
    add rdx, 16                                  ; Next 2 elements in array2
    add r8, 16                                   ; Next 2 positions in result
    dec r9                                       ; Decrement loop counter
    jnz sse_loop
    
    ret
sse_array_add ENDP
```

## Problem
Implement efficient array processing algorithms with pointer arithmetic.

### Skills Covered
- Multi-dimensional array access
- Pointer arithmetic for array traversal
- Search and sort algorithms
- In-place array manipulation

### Your Task
Create comprehensive array processing library:

**tutorial7.asm**:
```assembly
; Tutorial 7: Array processing and pointer arithmetic
.code

ExitProcess PROTO
printf PROTO

; TODO: Implement these array functions
; array_sum PROC
; find_min_max PROC
; linear_search PROC
; bubble_sort PROC
; matrix_multiply PROC

.data
    test_array    dq 64, 34, 25, 12, 22, 11, 90, 5
    array_size    equ ($ - test_array) / 8
    search_value  dq 22
    
    matrix_a      dd 1, 2, 3, 4, 5, 6           ; 2x3 matrix
    matrix_b      dd 7, 8, 9, 10, 11, 12        ; 2x3 matrix  
    matrix_result dd 6 dup(0)                   ; 2x3 result matrix
    
    sum_msg       db 'Array sum: %lld', 0Dh, 0Ah, 0
    minmax_msg    db 'Min: %lld at index %lld, Max: %lld at index %lld', 0Dh, 0Ah, 0
    search_msg    db 'Value %lld found at index: %lld', 0Dh, 0Ah, 0
    sorted_msg    db 'Array sorted successfully', 0Dh, 0Ah, 0

.code
main PROC
    sub rsp, 32                                  ; Shadow space
    
    ; TODO: Test your array functions
    ; 1. Calculate sum of test_array
    ; 2. Find minimum and maximum values with indices
    ; 3. Search for search_value in array
    ; 4. Sort the array using bubble sort
    ; 5. Demonstrate matrix operations
    
    ; Example test framework:
    lea rcx, test_array                          ; Array pointer
    mov rdx, array_size                          ; Array size
    call array_sum                               ; Your implementation
    
    ; Print sum
    lea rcx, sum_msg
    mov rdx, rax                                 ; Sum result
    call printf
    
    ; Test search
    lea rcx, test_array
    mov rdx, array_size  
    mov r8, search_value
    call linear_search                           ; Your implementation
    
    ; Print search result
    lea rcx, search_msg
    mov rdx, search_value
    mov r8, rax                                  ; Index result
    call printf
    
    ; Additional tests...
    
    add rsp, 32
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Expected Output:** Array processing results, search outcomes, sorting confirmation

---

# Tutorial 8: Bitwise Operations and Bit Manipulation

## Lesson: Bit-Level Operations and Efficient Algorithms

### Understanding Binary Representation
Every integer is stored as a sequence of bits in two's complement format:

```
Positive numbers (42 in binary):
00000000 00000000 00000000 00101010
||||||||                   ||||||||
MSB (sign bit = 0)         LSB

Negative numbers (-42 in binary, two's complement):
11111111 11111111 11111111 11010110
||||||||                   ||||||||
MSB (sign bit = 1)         LSB
```

### Core Bitwise Instructions

#### Logical Operations
```assembly
and rax, rbx                ; Bitwise AND: RAX = RAX & RBX
or  rax, rbx                ; Bitwise OR:  RAX = RAX | RBX  
xor rax, rbx                ; Bitwise XOR: RAX = RAX ^ RBX
not rax                     ; Bitwise NOT: RAX = ~RAX (one's complement)
```

#### Shift Operations
```assembly
shl rax, count              ; Shift Left:  RAX = RAX << count
shr rax, count              ; Shift Right: RAX = RAX >> count (logical, fills with 0)
sar rax, count              ; Arithmetic Right: RAX = RAX >> count (preserves sign)

; Shift operations with CL register (variable shift count)
mov cl, 3
shl rax, cl                 ; Shift RAX left by 3 positions
```

#### Rotation Operations
```assembly
rol rax, count              ; Rotate Left:  bits wrap around from MSB to LSB
ror rax, count              ; Rotate Right: bits wrap around from LSB to MSB

; Example: ROL with 8-bit value 10110001
; ROL by 1: 01100011 (leftmost bit wraps to rightmost)
```

### Bit Manipulation Fundamentals

#### Setting, Clearing, and Testing Bits
```assembly
; Set bit N: value |= (1 << N)
set_bit PROC
    ; Input: RCX = value, RDX = bit position
    ; Output: RAX = value with bit N set
    
    mov rax, 1
    mov cl, dl                      ; Bit position to CL
    shl rax, cl                     ; Create mask: 1 << N
    or rax, rcx                     ; Set the bit
    ret
set_bit ENDP

; Clear bit N: value &= ~(1 << N)  
clear_bit PROC
    ; Input: RCX = value, RDX = bit position
    ; Output: RAX = value with bit N cleared
    
    mov rax, 1
    mov cl, dl                      ; Bit position to CL
    shl rax, cl                     ; Create mask: 1 << N
    not rax                         ; Invert mask: ~(1 << N)
    and rax, rcx                    ; Clear the bit
    ret
clear_bit ENDP

; Test bit N: (value & (1 << N)) != 0
test_bit PROC
    ; Input: RCX = value, RDX = bit position
    ; Output: RAX = 1 if bit is set, 0 if clear
    
    mov rax, 1
    mov cl, dl                      ; Bit position to CL
    shl rax, cl                     ; Create mask: 1 << N
    test rcx, rax                   ; Test the bit
    setnz al                        ; Set AL to 1 if NZ, 0 if Z
    movzx rax, al                   ; Zero-extend to full register
    ret
test_bit ENDP
```

### Advanced Bit Manipulation Algorithms

#### Population Count (Count 1 bits)
```assembly
; Brian Kernighan's algorithm: n & (n-1) clears the lowest set bit
popcount PROC
    ; Input: RCX = value
    ; Output: RAX = number of 1 bits
    
    xor rax, rax                    ; Count = 0
    test rcx, rcx                   ; Check if value is 0
    jz popcount_done
    
popcount_loop:
    inc rax                         ; Increment count
    mov rdx, rcx                    ; Copy value
    dec rdx                         ; rdx = value - 1  
    and rcx, rdx                    ; Clear lowest set bit
    jnz popcount_loop               ; Continue if more bits set
    
popcount_done:
    ret
popcount ENDP

; Alternative: Using x64 POPCNT instruction (if available)
popcount_hw PROC
    ; Input: RCX = value
    ; Output: RAX = number of 1 bits
    
    popcnt rax, rcx                 ; Hardware population count
    ret
popcount_hw ENDP
```

#### Bit Field Extraction
```assembly
extract_bits PROC
    ; Input: RCX = value, RDX = start bit, R8 = number of bits
    ; Output: RAX = extracted bit field
    
    ; Shift right to align desired bits to LSB
    mov cl, dl                      ; Start bit position
    mov rax, rcx                    ; Value to extract from
    shr rax, cl                     ; Shift right by start position
    
    ; Create mask for desired number of bits
    mov rcx, 1
    mov cl, r8b                     ; Number of bits
    shl rcx, cl                     ; 2^(number of bits)
    dec rcx                         ; Create mask: 2^n - 1
    
    and rax, rcx                    ; Apply mask
    ret
extract_bits ENDP
```

#### Bit Reversal
```assembly
reverse_bits PROC
    ; Input: RCX = 64-bit value
    ; Output: RAX = bit-reversed value
    
    xor rax, rax                    ; Result = 0
    mov rdx, 64                     ; Bit counter
    
reverse_loop:
    shl rax, 1                      ; Shift result left
    shr rcx, 1                      ; Shift input right
    jnc skip_set                    ; Jump if no carry (bit was 0)
    or rax, 1                       ; Set LSB if carry (bit was 1)
    
skip_set:
    dec rdx                         ; Decrement counter
    jnz reverse_loop                ; Continue if more bits
    
    ret
reverse_bits ENDP
```

### Power-of-2 Detection and Operations

#### Check if Power of 2
```assembly
is_power_of_2 PROC
    ; Input: RCX = value
    ; Output: RAX = 1 if power of 2, 0 otherwise
    ; Algorithm: (n != 0) && ((n & (n-1)) == 0)
    
    test rcx, rcx                   ; Check if value is 0
    jz not_power_of_2               ; 0 is not a power of 2
    
    mov rax, rcx                    ; Copy value
    dec rax                         ; rax = value - 1
    and rax, rcx                    ; value & (value - 1)
    setz al                         ; Set AL to 1 if zero, 0 otherwise
    movzx rax, al                   ; Zero-extend to full register
    ret
    
not_power_of_2:
    xor rax, rax                    ; Return 0
    ret
is_power_of_2 ENDP
```

#### Find Next Power of 2
```assembly
next_power_of_2 PROC
    ; Input: RCX = value
    ; Output: RAX = smallest power of 2 >= value
    
    test rcx, rcx                   ; Check if 0
    jz return_one                   ; Next power of 2 after 0 is 1
    
    dec rcx                         ; n = n - 1
    
    ; Fill all bits to the right of the highest set bit
    mov rax, rcx
    shr rax, 1
    or rcx, rax                     ; n |= n >> 1
    
    mov rax, rcx  
    shr rax, 2
    or rcx, rax                     ; n |= n >> 2
    
    mov rax, rcx
    shr rax, 4  
    or rcx, rax                     ; n |= n >> 4
    
    mov rax, rcx
    shr rax, 8
    or rcx, rax                     ; n |= n >> 8
    
    mov rax, rcx
    shr rax, 16
    or rcx, rax                     ; n |= n >> 16
    
    mov rax, rcx
    shr rax, 32
    or rcx, rax                     ; n |= n >> 32
    
    inc rcx                         ; n = n + 1
    mov rax, rcx                    ; Return result
    ret
    
return_one:
    mov rax, 1                      ; Return 1
    ret
next_power_of_2 ENDP
```

### Bit Scanning Operations

#### Find First/Last Set Bit
```assembly
; Using x64 bit scan instructions
find_first_set PROC
    ; Input: RCX = value
    ; Output: RAX = position of first set bit (0-63), or -1 if none
    
    bsf rax, rcx                    ; Bit Scan Forward
    jz no_bits_set                  ; Jump if no bits were set
    ret                             ; RAX contains bit position
    
no_bits_set:
    mov rax, -1                     ; Return -1 if no bits set
    ret
find_first_set ENDP

find_last_set PROC
    ; Input: RCX = value  
    ; Output: RAX = position of last set bit (0-63), or -1 if none
    
    bsr rax, rcx                    ; Bit Scan Reverse
    jz no_bits_set_last             ; Jump if no bits were set
    ret                             ; RAX contains bit position
    
no_bits_set_last:
    mov rax, -1                     ; Return -1 if no bits set
    ret
find_last_set ENDP

; Count leading zeros
count_leading_zeros PROC
    ; Input: RCX = value
    ; Output: RAX = number of leading zeros
    
    lzcnt rax, rcx                  ; Leading Zero Count (modern CPUs)
    ret
count_leading_zeros ENDP
```

### Bit Manipulation Tricks and Optimizations

#### Fast Multiplication and Division by Powers of 2
```assembly
; Multiply by 2^n using left shift
multiply_by_power_of_2 PROC
    ; Input: RCX = value, RDX = power (n)
    ; Output: RAX = value * (2^n)
    
    mov rax, rcx                    ; Copy value
    mov cl, dl                      ; Power to CL register
    shl rax, cl                     ; Shift left by n positions
    ret
multiply_by_power_of_2 ENDP

; Divide by 2^n using right shift (unsigned)
divide_by_power_of_2 PROC
    ; Input: RCX = value, RDX = power (n)
    ; Output: RAX = value / (2^n)
    
    mov rax, rcx                    ; Copy value
    mov cl, dl                      ; Power to CL register
    shr rax, cl                     ; Shift right by n positions
    ret
divide_by_power_of_2 ENDP
```

#### Bit Masks and Filters
```assembly
; Create bit mask with N consecutive 1s
create_mask PROC
    ; Input: RCX = number of bits
    ; Output: RAX = mask with N lowest bits set
    
    cmp rcx, 64                     ; Check if >= 64 bits requested
    jae all_bits_set                ; Set all bits if >= 64
    
    mov rax, 1
    mov cl, cl                      ; Number of bits to CL
    shl rax, cl                     ; 2^N
    dec rax                         ; 2^N - 1 (N consecutive 1s)
    ret
    
all_bits_set:
    mov rax, -1                     ; All bits set
    ret
create_mask ENDP

; Apply bit mask to isolate specific bits
apply_mask PROC
    ; Input: RCX = value, RDX = mask
    ; Output: RAX = value & mask
    
    mov rax, rcx
    and rax, rdx                    ; Apply mask
    ret
apply_mask ENDP
```

### Cryptographic and Hash-Related Operations

#### Simple Hash Function Using Bit Operations
```assembly
simple_hash PROC
    ; Input: RCX = data, RDX = length
    ; Output: RAX = hash value
    
    xor rax, rax                    ; Hash = 0
    test rdx, rdx                   ; Check if length is 0
    jz hash_done
    
hash_loop:
    movzx r8, byte ptr [rcx]        ; Load byte
    xor rax, r8                     ; XOR with current byte
    rol rax, 1                      ; Rotate left by 1
    inc rcx                         ; Next byte
    dec rdx                         ; Decrement length
    jnz hash_loop                   ; Continue if more bytes
    
hash_done:
    ret
simple_hash ENDP
```

#### Parity Calculation
```assembly
calculate_parity PROC
    ; Input: RCX = value
    ; Output: RAX = 1 if odd number of 1s, 0 if even
    
    ; Method 1: Using population count
    call popcount                   ; Get number of 1 bits
    and rax, 1                      ; Keep only LSB (odd/even)
    ret
    
    ; Method 2: XOR folding (alternative implementation)
    ; mov rax, rcx
    ; shr rcx, 32
    ; xor rax, rcx        ; Fold upper 32 bits into lower 32
    ; mov rcx, rax
    ; shr rax, 16  
    ; xor rax, rcx        ; Fold upper 16 bits into lower 16
    ; mov rcx, rax
    ; shr rax, 8
    ; xor rax, rcx        ; Fold upper 8 bits into lower 8
    ; mov rcx, rax
    ; shr rax, 4
    ; xor rax, rcx        ; Fold upper 4 bits into lower 4
    ; mov rcx, rax
    ; shr rax, 2
    ; xor rax, rcx        ; Fold upper 2 bits into lower 2
    ; mov rcx, rax
    ; shr rax, 1
    ; xor rax, rcx        ; Final fold
    ; and rax, 1          ; Keep only LSB
    ; ret
calculate_parity ENDP
```

### Windows-Specific Bit Operations

#### Using Interlocked Bit Operations (Thread-Safe)
```assembly
; Windows provides interlocked operations for thread-safe bit manipulation
; These are typically done through Windows API calls:

.data
    shared_flags dq 0               ; Shared between threads

.code
atomic_set_bit PROC
    ; Input: RCX = bit position
    ; Atomically sets bit in shared_flags
    
    mov rax, 1
    mov cl, cl                      ; Bit position
    shl rax, cl                     ; Create bit mask
    
    ; Use Windows InterlockedOr64 API
    lea rcx, shared_flags           ; Address of shared variable
    mov rdx, rax                    ; Bit mask
    call InterlockedOr64            ; Atomic OR operation
    
    ret
atomic_set_bit ENDP
```

## Problem
Implement efficient bit manipulation algorithms using x64 bit operations.

### Skills Covered
- Population count (number of 1 bits)
- Bit field extraction and manipulation
- Bit rotation and shifting operations
- Power-of-2 detection and operations

### Your Task
Create comprehensive bit manipulation library:

**tutorial8.asm**:
```assembly
; Tutorial 8: Bitwise operations and bit manipulation
.code

ExitProcess PROTO
printf PROTO

; TODO: Implement these bit manipulation functions
; popcount PROC
; extract_bits PROC
; set_bit PROC
; clear_bit PROC
; toggle_bit PROC
; is_power_of_2 PROC
; rotate_left PROC
; reverse_bits PROC

.data
    test_values dq 0x12345678ABCDEF00, 0xFF00FF00FF00FF00, 0x8000000000000001
    test_count equ ($ - test_values) / 8
    
    popcount_msg    db 'Value: %016llX, Population count: %lld', 0Dh, 0Ah, 0
    extract_msg     db 'Extracted bits [%lld:%lld] = %016llX', 0Dh, 0Ah, 0
    power2_msg      db 'Value %lld is %spower of 2', 0Dh, 0Ah, 0
    rotate_msg      db 'Rotated left by %lld: %016llX', 0Dh, 0Ah, 0
    not_str         db 'NOT a ', 0
    empty_str       db 0

.code
main PROC
    sub rsp, 32                     ; Shadow space
    
    ; TODO: Test your bit manipulation functions
    mov rbx, 0                      ; Index for test values
    
test_loop:
    cmp rbx, test_count
    jge test_done
    
    ; Get current test value
    mov rax, qword ptr [test_values + rbx*8]
    push rax                        ; Save test value
    push rbx                        ; Save index
    
    ; Test population count
    mov rcx, rax
    call popcount                   ; Your implementation
    
    lea rcx, popcount_msg
    pop rbx                         ; Restore index
    pop rdx                         ; Restore test value
    push rdx                        ; Save again
    push rbx                        ; Save again
    mov r8, rax                     ; Popcount result
    call printf
    
    ; Test bit extraction (extract bits 8-15)
    pop rbx                         ; Restore index
    pop rax                         ; Restore test value
    push rax                        ; Save again
    push rbx                        ; Save again
    
    mov rcx, rax                    ; Value
    mov rdx, 8                      ; Start bit
    mov r8, 8                       ; Number of bits
    call extract_bits               ; Your implementation
    
    lea rcx, extract_msg
    mov rdx, 8                      ; Start bit
    mov r8, 8                       ; Number of bits
    mov r9, rax                     ; Extracted value
    call printf
    
    ; Test power of 2 detection
    pop rbx                         ; Restore index
    pop rax                         ; Restore test value
    
    ; Test with various values including the test value
    mov rcx, rax
    call is_power_of_2              ; Your implementation
    
    ; Print result
    lea r8, empty_str               ; Assume it is power of 2
    test rax, rax
    jnz print_power2
    lea r8, not_str                 ; It's not a power of 2
    
print_power2:
    lea rcx, power2_msg
    mov rdx, qword ptr [test_values + rbx*8]
    call printf
    
    inc rbx                         ; Next test value
    jmp test_loop
    
test_done:
    ; Additional specific tests
    ; Test rotation
    mov rcx, 0x123456789ABCDEF0
    mov rdx, 4                      ; Rotate left by 4
    call rotate_left                ; Your implementation
    
    lea rcx, rotate_msg
    mov rdx, 4                      ; Rotation amount
    mov r8, rax                     ; Rotated result
    call printf
    
    add rsp, 32
    mov rcx, 0
    call ExitProcess
main ENDP

END
```

**Expected Output:** Bit manipulation results, power-of-2 detection, rotation effects

---

This completes the first 8 tutorials of the Windows MASM version. Each tutorial builds upon the previous ones, teaching progressive assembly concepts while using Windows-specific calling conventions, APIs, and MASM syntax.