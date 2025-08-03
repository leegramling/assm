// tutorial11.cpp - Understanding and debugging optimized code
#include <iostream>
#include <vector>
#include <chrono>

// Function that demonstrates various optimization opportunities
int simple_loop(const std::vector<int>& data) {
    int sum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
    }
    return sum;
}

// Function with constant that can be folded
int constant_calculation() {
    int a = 10;
    int b = 20;
    int c = 30;
    return a + b + c;  // Compiler can compute this at compile time
}

// Function that can be inlined
inline int add_numbers(int x, int y) {
    return x + y;
}

// Function that calls the inline function multiple times
int use_inline_function(int base) {
    int result = base;
    result = add_numbers(result, 5);
    result = add_numbers(result, 10);
    result = add_numbers(result, 15);
    return result;
}

// Function with predictable branch that can be optimized
int conditional_sum(const std::vector<int>& data, bool always_true) {
    int sum = 0;
    for (const auto& value : data) {
        if (always_true) {  // Compiler may optimize this branch away
            sum += value;
        }
    }
    return sum;
}

// Function with loop that can be unrolled
void copy_array(const int* src, int* dest, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dest[i] = src[i];
    }
}

// Function with redundant calculations that can be eliminated
int redundant_calculations(int x) {
    int y = x * 2;
    int z = x * 2;  // Same as y, can be optimized
    return y + z;   // Can become x * 4
}

// Function to demonstrate common optimization patterns
class OptimizationDemo {
private:
    int value;
    
public:
    OptimizationDemo(int val) : value(val) {}
    
    // Small function that will likely be inlined
    int get_value() const { return value; }
    
    // Function with loop that might be vectorized
    void multiply_array(std::vector<float>& data, float multiplier) {
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] *= multiplier;
        }
    }
};

// Template function that will be instantiated
template<typename T>
T max_value(T a, T b) {
    return (a > b) ? a : b;
}

void benchmark_optimization_effects() {
    const size_t DATA_SIZE = 1000000;
    std::vector<int> large_data(DATA_SIZE, 1);
    std::vector<float> float_data(DATA_SIZE, 2.5f);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Operations that show optimization effects
    int sum = simple_loop(large_data);
    int constant = constant_calculation();
    int inline_result = use_inline_function(100);
    int conditional = conditional_sum(large_data, true);
    
    OptimizationDemo demo(42);
    int value = demo.get_value();
    demo.multiply_array(float_data, 1.5f);
    
    // Template instantiation
    int max_int = max_value(10, 20);
    float max_float = max_value(3.14f, 2.71f);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Results:" << std::endl;
    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Constant: " << constant << std::endl;
    std::cout << "Inline result: " << inline_result << std::endl;
    std::cout << "Conditional sum: " << conditional << std::endl;
    std::cout << "Object value: " << value << std::endl;
    std::cout << "Max int: " << max_int << std::endl;
    std::cout << "Max float: " << max_float << std::endl;
    std::cout << "Time: " << duration.count() << " microseconds" << std::endl;
}

int main() {
    std::cout << "=== Optimization Analysis Tutorial ===" << std::endl;
    std::cout << "Compile with different optimization levels to see effects:" << std::endl;
    std::cout << "  -O0: No optimization" << std::endl;
    std::cout << "  -O1: Basic optimization" << std::endl;
    std::cout << "  -O2: Standard optimization" << std::endl;
    std::cout << "  -O3: Aggressive optimization" << std::endl;
    std::cout << std::endl;
    
    benchmark_optimization_effects();
    
    std::cout << std::endl;
    std::cout << "To analyze optimizations:" << std::endl;
    std::cout << "1. Compile with different -O levels" << std::endl;
    std::cout << "2. Use 'objdump -d tutorial11' to see assembly" << std::endl;
    std::cout << "3. Use 'gdb tutorial11' and 'disas function_name'" << std::endl;
    std::cout << "4. Compare assembly output between optimization levels" << std::endl;
    
    return 0;
}