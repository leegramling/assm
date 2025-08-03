// tutorial10_complete.cpp - Mixed C++/Assembly programming
#include <iostream>
#include <string>

extern "C" long multiply_add_asm(long a, long b, long c);

// Assembly function: (a * b) + c
__asm__(
    ".globl multiply_add_asm\n"
    "multiply_add_asm:\n"
    "pushq %rbp\n\t"             // Save base pointer
    "movq %rsp, %rbp\n\t"        // Set up frame
    "movq %rdi, %rax\n\t"        // Load a into RAX
    "mulq %rsi\n\t"              // Multiply RAX by b (RSI)
    "addq %rdx, %rax\n\t"        // Add c (RDX) to result
    "popq %rbp\n\t"              // Restore base pointer
    "ret\n"
);

class Calculator {
private:
    int value;
    
public:
    Calculator(int initial_value) : value(initial_value) {}
    
    int multiply_and_add(int a, int b) {
        // Call assembly function and add to current value
        long asm_result = multiply_add_asm(a, b, 0);  // (a * b) + 0
        return static_cast<int>(asm_result) + value;   // Add to object's value
    }
    
    int get_value() const { return value; }
    void set_value(int new_value) { value = new_value; }
};

extern "C" void cpp_print_number(long number) {
    std::cout << number << std::endl;
}

extern "C" void demo_callback_asm(long number);

__asm__(
    ".globl demo_callback_asm\n"
    "demo_callback_asm:\n"
    "pushq %rbp\n\t"             // Save base pointer
    "movq %rsp, %rbp\n\t"        // Set up frame
    "pushq %rbx\n\t"             // Save callee-saved register
    "movq %rdi, %rbx\n\t"        // Save parameter in callee-saved register
    "movq %rbx, %rdi\n\t"        // Set up parameter for C++ function
    "call cpp_print_number\n\t"  // Call C++ function
    "popq %rbx\n\t"              // Restore callee-saved register
    "popq %rbp\n\t"              // Restore base pointer
    "ret\n"
);

long complex_calculation(long x, long y, long z) {
    long result;
    
    __asm__ volatile (
        "movq %1, %%rax\n\t"        // Load x into RAX
        "mulq %2\n\t"               // Multiply by y
        "movq %%rax, %%rbx\n\t"     // Save x*y in RBX
        "movq %3, %%rax\n\t"        // Load z into RAX
        "shlq $1, %%rax\n\t"        // Multiply z by 2 (left shift)
        "addq %%rbx, %%rax\n\t"     // Add (x*y) + (z*2)
        "movq %%rax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (x), "m" (y), "m" (z)
        : "rax", "rbx", "rdx"       // mulq clobbers rdx
    );
    
    return result;
}

void demonstrate_cpp_features() {
    Calculator calc(10);
    
    // Method call - this becomes: calc.multiply_and_add(&calc, 3, 4)
    // Where &calc is passed as hidden 'this' parameter in RDI
    int result = calc.multiply_and_add(3, 4);
    
    // Member access - direct memory access with offset
    int current_value = calc.get_value();
    
    std::cout << "Calculator result: " << result << std::endl;      // Should print: 22 (3*4 + 10)
    std::cout << "Current value: " << current_value << std::endl;   // Should print: 10
}

int main() {
    std::cout << "=== Mixed C++/Assembly Tutorial ===" << std::endl;
    
    // Test assembly function called from C++
    long asm_result = multiply_add_asm(5, 6, 10);
    std::cout << "Assembly function result (5*6+10): " << asm_result << std::endl;  // Should print: 40
    
    // Test C++ class with assembly integration
    Calculator calc(100);
    int calc_result = calc.multiply_and_add(7, 8);
    std::cout << "Calculator result (7*8+100): " << calc_result << std::endl;       // Should print: 156
    
    // Test assembly calling C++ function
    std::cout << "Assembly calling C++ function: ";
    demo_callback_asm(42);                                                          // Should print: 42
    
    // Test inline assembly in C++
    long inline_result = complex_calculation(3, 4, 5);
    std::cout << "Complex calculation (3*4 + 5*2): " << inline_result << std::endl; // Should print: 22
    
    // Demonstrate C++ features for analysis
    demonstrate_cpp_features();
    
    return 0;
}