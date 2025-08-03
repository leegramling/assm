// tutorial10.cpp - Mixed C++/Assembly programming
#include <iostream>
#include <string>

// TODO: Implement an assembly function callable from C++
extern "C" long multiply_add_asm(long a, long b, long c);

// TODO: Create assembly implementation
// Assembly function should calculate: (a * b) + c
__asm__(
    ".globl multiply_add_asm\n"
    "multiply_add_asm:\n"
    // TODO: Your assembly implementation here
    // Parameters: RDI=a, RSI=b, RDX=c
    // Return value in RAX
    "ret\n"
);

// Simple C++ class for demonstration
class Calculator {
private:
    int value;
    
public:
    Calculator(int initial_value) : value(initial_value) {}
    
    // TODO: Implement this method to call assembly function
    int multiply_and_add(int a, int b) {
        // TODO: Call the assembly function and add to current value
        // Return: (a * b) + this->value
        return 0;
    }
    
    int get_value() const { return value; }
    void set_value(int new_value) { value = new_value; }
};

// TODO: Implement C++ function that can be called from assembly
extern "C" void cpp_print_number(long number) {
    // TODO: Print the number using C++ iostream
    // This shows how assembly can call back into C++
}

// TODO: Assembly function that calls C++ function
extern "C" void demo_callback_asm(long number);

__asm__(
    ".globl demo_callback_asm\n"
    "demo_callback_asm:\n"
    // TODO: Your assembly implementation here
    // 1. Save registers (following calling convention)
    // 2. Call cpp_print_number with the parameter
    // 3. Restore registers
    // 4. Return
    "ret\n"
);

// TODO: Function to demonstrate C++ to assembly parameter passing
long complex_calculation(long x, long y, long z) {
    long result;
    // TODO: Use inline assembly to:
    // 1. Take three C++ parameters
    // 2. Perform calculation: result = (x * y) + (z * 2)
    // 3. Return result to C++
    
    return result;
}

// Function to help analyze assembly output
void demonstrate_cpp_features() {
    // Create object on stack
    Calculator calc(10);
    
    // Method call - see how this translates to assembly
    int result = calc.multiply_and_add(3, 4);
    
    // Member access
    int current_value = calc.get_value();
    
    // Virtual function call would be more complex
    std::cout << "Calculator result: " << result << std::endl;
    std::cout << "Current value: " << current_value << std::endl;
}

int main() {
    std::cout << "=== Mixed C++/Assembly Tutorial ===" << std::endl;
    
    // Test assembly function called from C++
    long asm_result = multiply_add_asm(5, 6, 10);
    std::cout << "Assembly function result (5*6+10): " << asm_result << std::endl;
    
    // Test C++ class with assembly integration
    Calculator calc(100);
    int calc_result = calc.multiply_and_add(7, 8);
    std::cout << "Calculator result (7*8+100): " << calc_result << std::endl;
    
    // Test assembly calling C++ function
    std::cout << "Assembly calling C++ function: ";
    demo_callback_asm(42);
    
    // Test inline assembly in C++
    long inline_result = complex_calculation(3, 4, 5);
    std::cout << "Complex calculation (3*4 + 5*2): " << inline_result << std::endl;
    
    // Demonstrate C++ features for analysis
    demonstrate_cpp_features();
    
    return 0;
}