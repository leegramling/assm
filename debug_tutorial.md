# Tutorial 5: Debugging C++ Libraries with GDB

## Objective
Learn to debug a C++ library function using only assembly-level debugging when source code is not available.

## Files Provided
- `library.cpp` - Library source (you won't look at this during debugging)
- `main.cpp` - Main program that calls the library function

## Your Task
1. Compile the library without debug symbols (simulating a stripped library)
2. Use GDB to trace through the execution at assembly level
3. Determine what the `mysterious_function` does without looking at source

## Commands to Compile and Debug

### Step 1: Compile
```bash
# Compile library without debug info (stripped)
g++ -c -O0 library.cpp -o library.o

# Compile main with debug info
g++ -g -O0 main.cpp library.o -o debug_example
```

### Step 2: Debug with GDB
```bash
gdb ./debug_example
```

### Step 3: GDB Commands to Use
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

## Expected Discovery Process
1. Function takes two parameters (a=10, b=15)
2. First operation: multiply first parameter by 2 (temp = 20)
3. Compare temp with second parameter (20 vs 15)
4. Since 20 > 15, return temp + b (20 + 15 = 35)

## Expected Output
```
Result: 35
```

This exercise teaches you to reverse engineer library functionality using only assembly debugging techniques.