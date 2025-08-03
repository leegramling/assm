// tutorial9.c - Floating point operations with SSE
#include <stdio.h>
#include <math.h>

// TODO: Implement basic floating point arithmetic using SSE
float add_floats_sse(float a, float b) {
    float result;
    // TODO: Use SSE assembly to:
    // 1. Load floats into XMM registers using movss
    // 2. Add them using addss instruction
    // 3. Store result back to memory
    
    return result;
}

// TODO: Implement floating point comparison using SSE
int compare_floats_sse(float a, float b) {
    int result;
    // TODO: Use SSE assembly to:
    // 1. Load floats into XMM registers
    // 2. Compare using comiss instruction
    // 3. Set result based on comparison flags
    // Return: -1 if a < b, 0 if a == b, 1 if a > b
    
    return result;
}

// TODO: Implement square root using SSE
float sqrt_sse(float value) {
    float result;
    // TODO: Use SSE assembly to:
    // 1. Load value into XMM register
    // 2. Use sqrtss instruction
    // 3. Store result back
    
    return result;
}

// TODO: Implement vector addition (4 floats at once) using SSE
void add_vectors_sse(const float* a, const float* b, float* result) {
    // TODO: Use SSE assembly to:
    // 1. Load 4 floats from each array using movups
    // 2. Add vectors using addps (packed single precision)
    // 3. Store result using movups
    // This processes 4 floats simultaneously
}

// TODO: Implement dot product using SSE
float dot_product_sse(const float* a, const float* b, int count) {
    float result;
    // TODO: Use SSE assembly to:
    // 1. Process vectors 4 elements at a time
    // 2. Use mulps for multiplication
    // 3. Use horizontal add or extract elements for final sum
    // 4. Handle remaining elements if count % 4 != 0
    
    return result;
}

// TODO: Implement fast reciprocal square root approximation
float fast_inv_sqrt_sse(float value) {
    float result;
    // TODO: Use SSE assembly to:
    // 1. Use rsqrtss instruction for approximation
    // 2. Optional: Use Newton-Raphson iteration for better precision
    
    return result;
}

int main() {
    float a = 3.14159f;
    float b = 2.71828f;
    
    printf("a = %.5f, b = %.5f\n", a, b);
    printf("a + b = %.5f\n", add_floats_sse(a, b));
    printf("sqrt(a) = %.5f\n", sqrt_sse(a));
    printf("compare(a, b) = %d\n", compare_floats_sse(a, b));
    
    // Vector operations
    float vec1[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float vec2[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    float vec_result[4];
    
    add_vectors_sse(vec1, vec2, vec_result);
    printf("Vector addition: [%.1f, %.1f, %.1f, %.1f]\n", 
           vec_result[0], vec_result[1], vec_result[2], vec_result[3]);
    
    printf("Dot product: %.1f\n", dot_product_sse(vec1, vec2, 4));
    
    printf("Fast inv sqrt(4.0) = %.5f\n", fast_inv_sqrt_sse(4.0f));
    printf("Expected 1/sqrt(4.0) = %.5f\n", 1.0f / sqrtf(4.0f));
    
    return 0;
}