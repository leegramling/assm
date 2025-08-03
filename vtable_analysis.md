# Tutorial 12: Understanding C++ Object Layout and VTables

## Overview
This tutorial explains how C++ objects are laid out in memory and how virtual function calls work at the assembly level. This knowledge is crucial for debugging complex C++ libraries with inheritance hierarchies.

## C++ Object Memory Layout

### Simple Class Layout
```cpp
class SimpleClass {
private:
    int value1;    // Offset 0
    int value2;    // Offset 4
public:
    void method(); // No storage in object
};
```

**Memory Layout:**
```
[value1: 4 bytes][value2: 4 bytes] = 8 bytes total
```

### Class with Virtual Functions
```cpp
class Base {
private:
    int data;           // Offset 8 (after vtable pointer)
public:
    virtual void func();
};
```

**Memory Layout:**
```
[vtable ptr: 8 bytes][data: 4 bytes][padding: 4 bytes] = 16 bytes total
```

## Virtual Function Tables (VTables)

### VTable Structure
Each class with virtual functions has a vtable:
```
Base vtable:
[0] -> Base::~Base()
[1] -> Base::virtual_method()
[2] -> [pure virtual - null or abort]

Derived vtable:
[0] -> Derived::~Derived()
[1] -> Derived::virtual_method()  // Overridden
[2] -> Derived::pure_virtual_method()
```

### Virtual Function Call Assembly
```cpp
base_ptr->virtual_method();
```

**Translates to:**
```assembly
movq %rdi, %rax           # Load 'this' pointer (base_ptr)
movq (%rax), %rdx         # Load vtable pointer from object
movq 8(%rdx), %rdx        # Load function pointer from vtable[1]
movq %rax, %rdi           # Set 'this' as first parameter
call *%rdx                # Call virtual function
```

**Step-by-step breakdown:**
1. Load object address ('this' pointer)
2. Dereference to get vtable pointer (first 8 bytes of object)
3. Index into vtable to get function address
4. Call function with 'this' pointer as first parameter

## Inheritance Scenarios

### Single Inheritance
```cpp
class Base {
    int base_data;        // Offset 8
    virtual void func();
};

class Derived : public Base {
    int derived_data;     // Offset 12
};
```

**Memory Layout:**
```
[vtable ptr: 8][base_data: 4][derived_data: 4][padding: 4] = 20 bytes
```

### Multiple Inheritance
```cpp
class Interface1 {
    virtual void func1();
};

class Interface2 {
    virtual void func2();
};

class Multi : public Interface1, public Interface2 {
    int data;
};
```

**Memory Layout:**
```
[vtable1 ptr: 8][vtable2 ptr: 8][data: 4][padding: 4] = 24 bytes
```

**Pointer Adjustment:**
```cpp
Multi obj;
Interface1* ptr1 = &obj;     // No adjustment (offset 0)
Interface2* ptr2 = &obj;     // Adjusted by 8 bytes (offset 8)
```

## Assembly Patterns for Object-Oriented Code

### Constructor Call
```cpp
Derived obj(42, 84);
```

**Assembly pattern:**
```assembly
# Allocate space on stack
subq $16, %rsp            # Allocate space for object

# Call constructor
leaq (%rsp), %rdi         # Load 'this' pointer
movl $42, %esi            # First parameter
movl $84, %edx            # Second parameter
call _ZN7DerivedC1Eii     # Call Derived::Derived(int, int)
```

### Method Call (Non-Virtual)
```cpp
obj.non_virtual_method();
```

**Assembly pattern:**
```assembly
leaq (%rsp), %rdi         # Load 'this' pointer
call _ZN4Base18non_virtual_methodEv  # Direct call
```

### Method Call (Virtual)
```cpp
obj.virtual_method();
```

**Assembly pattern:**
```assembly
leaq (%rsp), %rdi         # Load 'this' pointer
movq (%rdi), %rax         # Load vtable pointer
call *8(%rax)             # Call vtable[1]
```

## GDB Debugging Techniques

### Examining Object Layout
```bash
(gdb) print obj
(gdb) print *&obj
(gdb) x/4x &obj           # Examine raw memory
```

### Examining VTable
```bash
(gdb) info vtbl obj       # Show vtable information
(gdb) print *obj          # Print object with vtable info
(gdb) x/8x *(void**)&obj  # Raw vtable examination
```

### Tracing Virtual Calls
```bash
(gdb) break virtual_method
(gdb) step
(gdb) disas              # See assembly for virtual call
(gdb) print $rdi         # 'this' pointer
(gdb) x/x $rdi           # vtable pointer
```

## Common Debugging Scenarios

### 1. Pure Virtual Function Call
**Symptom:** Crash with "pure virtual method called"
**Assembly:** Call to null pointer or abort function in vtable

### 2. Wrong Virtual Function Called
**Debug steps:**
1. Check object type: `print typeid(obj).name()`
2. Check vtable: `info vtbl obj`
3. Verify object construction

### 3. Multiple Inheritance Issues
**Debug steps:**
1. Check pointer values: Different interfaces have different addresses
2. Examine pointer adjustments in assembly
3. Use `dynamic_cast` to verify relationships

### 4. Memory Corruption
**Symptoms:** 
- Invalid vtable pointer
- Crashes in virtual function calls
- Wrong function called

**Debug approach:**
```bash
(gdb) watch *(void**)&object  # Watch vtable pointer
(gdb) x/8x object            # Examine object memory
```

## Optimization Effects on Object Layout

### Empty Base Optimization
```cpp
class Empty {};           // Size: 1 byte (minimum)
class Derived : Empty {   // Size: 4 bytes (no padding for Empty)
    int data;
};
```

### Virtual Function Optimization
- Compiler may devirtualize calls when type is known
- Final classes may have optimized vtables
- Link-time optimization may eliminate unused virtual functions

## Key Assembly Patterns to Recognize

### Virtual Function Call Pattern
```assembly
movq (%rdi), %rax         # Load vtable
call *offset(%rax)        # Call through vtable
```

### Constructor Pattern
```assembly
call constructor          # Initialize object
movq $vtable, (%rax)      # Set vtable pointer
```

### Dynamic Cast Pattern
```assembly
call __dynamic_cast       # Runtime type checking
testq %rax, %rax          # Check if cast succeeded
```

### Multiple Inheritance Adjustment
```assembly
addq $offset, %rax        # Adjust 'this' pointer
```

## Real-World Debugging Tips

1. **Identify virtual calls** by looking for vtable dereferencing
2. **Check object types** using RTTI or GDB's `info vtbl`
3. **Understand pointer adjustments** in multiple inheritance
4. **Watch for vtable corruption** - first sign of memory issues
5. **Use debug builds** for clearer object layouts
6. **Understand that optimizations** may eliminate virtual calls

## Memory Layout Examples

### Simple Class
```
Object: [8 bytes data] = 8 bytes total
```

### Virtual Base Class  
```
Object: [8 bytes vtable ptr][4 bytes data][4 bytes padding] = 16 bytes
```

### Derived Class
```
Object: [8 bytes vtable ptr][4 bytes base data][4 bytes derived data][4 bytes padding] = 20 bytes
```

### Multiple Inheritance
```
Object: [8 bytes vtable1][8 bytes vtable2][4 bytes data][4 bytes padding] = 24 bytes
```

Understanding these patterns helps decode complex C++ assembly and debug object-oriented code effectively.