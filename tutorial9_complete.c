// tutorial9_complete.c - Floating point operations with SSE
#include <stdio.h>
#include <math.h>

float add_floats_sse(float a, float b) {
    float result;
    
    __asm__ volatile (
        "movss %1, %%xmm0\n\t"      // Load a into XMM0
        "movss %2, %%xmm1\n\t"      // Load b into XMM1
        "addss %%xmm1, %%xmm0\n\t"  // Add: XMM0 = XMM0 + XMM1
        "movss %%xmm0, %0\n\t"      // Store result
        : "=m" (result)
        : "m" (a), "m" (b)
        : "xmm0", "xmm1"
    );
    
    return result;
}

int compare_floats_sse(float a, float b) {
    int result;
    
    __asm__ volatile (
        "movss %1, %%xmm0\n\t"      // Load a into XMM0
        "movss %2, %%xmm1\n\t"      // Load b into XMM1
        "comiss %%xmm1, %%xmm0\n\t" // Compare XMM0 with XMM1
        "movl $0, %%eax\n\t"        // Default result = 0 (equal)
        "je 2f\n\t"                 // If equal, done
        "movl $-1, %%eax\n\t"       // Assume a < b
        "jb 2f\n\t"                 // If below (a < b), done
        "movl $1, %%eax\n\t"        // Otherwise a > b
        "2:\n\t"                    // end
        "movl %%eax, %0\n\t"        // Store result
        : "=m" (result)
        : "m" (a), "m" (b)
        : "eax", "xmm0", "xmm1"
    );
    
    return result;
}

float sqrt_sse(float value) {
    float result;
    
    __asm__ volatile (
        "movss %1, %%xmm0\n\t"      // Load value into XMM0
        "sqrtss %%xmm0, %%xmm0\n\t" // Square root: XMM0 = sqrt(XMM0)
        "movss %%xmm0, %0\n\t"      // Store result
        : "=m" (result)
        : "m" (value)
        : "xmm0"
    );
    
    return result;
}

void add_vectors_sse(const float* a, const float* b, float* result) {
    __asm__ volatile (
        "movups (%0), %%xmm0\n\t"   // Load 4 floats from array a
        "movups (%1), %%xmm1\n\t"   // Load 4 floats from array b
        "addps %%xmm1, %%xmm0\n\t"  // Add packed singles: XMM0 = XMM0 + XMM1
        "movups %%xmm0, (%2)\n\t"   // Store result to output array
        :
        : "r" (a), "r" (b), "r" (result)
        : "xmm0", "xmm1", "memory"
    );
}

float dot_product_sse(const float* a, const float* b, int count) {
    float result;
    
    __asm__ volatile (
        "xorps %%xmm0, %%xmm0\n\t"  // Clear accumulator
        "movl %2, %%ecx\n\t"        // Load count
        "shrl $2, %%ecx\n\t"        // Divide by 4 (process 4 floats at a time)
        "jz 2f\n\t"                 // If less than 4 elements, skip vector loop
        "1:\n\t"                    // vector_loop
        "movups (%0), %%xmm1\n\t"   // Load 4 floats from a
        "movups (%1), %%xmm2\n\t"   // Load 4 floats from b
        "mulps %%xmm2, %%xmm1\n\t"  // Multiply: XMM1 = XMM1 * XMM2
        "addps %%xmm1, %%xmm0\n\t"  // Add to accumulator
        "addq $16, %0\n\t"          // Advance pointer a by 16 bytes (4 floats)
        "addq $16, %1\n\t"          // Advance pointer b by 16 bytes
        "decl %%ecx\n\t"            // Decrement counter
        "jnz 1b\n\t"                // Continue if not zero
        "2:\n\t"                    // sum_vector
        // Horizontal sum of XMM0
        "movaps %%xmm0, %%xmm1\n\t" // Copy XMM0 to XMM1
        "shufps $0x4E, %%xmm1, %%xmm1\n\t" // Shuffle high 64 bits to low
        "addps %%xmm1, %%xmm0\n\t"  // Add high and low parts
        "movaps %%xmm0, %%xmm1\n\t" // Copy again
        "shufps $0xB1, %%xmm1, %%xmm1\n\t" // Shuffle remaining elements
        "addss %%xmm1, %%xmm0\n\t"  // Final sum in XMM0[0]
        "movss %%xmm0, %3\n\t"      // Store result
        : "=r" (a), "=r" (b)
        : "m" (count), "m" (result), "0" (a), "1" (b)
        : "ecx", "xmm0", "xmm1", "xmm2", "memory"
    );
    
    return result;
}

float fast_inv_sqrt_sse(float value) {
    float result;
    
    __asm__ volatile (
        "movss %1, %%xmm0\n\t"      // Load value into XMM0
        "rsqrtss %%xmm0, %%xmm0\n\t" // Reciprocal square root approximation
        "movss %%xmm0, %0\n\t"      // Store result
        : "=m" (result)
        : "m" (value)
        : "xmm0"
    );
    
    return result;
}

int main() {
    float a = 3.14159f;
    float b = 2.71828f;
    
    printf("a = %.5f, b = %.5f\n", a, b);
    printf("a + b = %.5f\n", add_floats_sse(a, b));                        // Should print: 5.85987
    printf("sqrt(a) = %.5f\n", sqrt_sse(a));                               // Should print: 1.77245
    printf("compare(a, b) = %d\n", compare_floats_sse(a, b));              // Should print: 1 (a > b)
    
    // Vector operations
    float vec1[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float vec2[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    float vec_result[4];
    
    add_vectors_sse(vec1, vec2, vec_result);
    printf("Vector addition: [%.1f, %.1f, %.1f, %.1f]\n",                  // Should print: [6.0, 8.0, 10.0, 12.0]
           vec_result[0], vec_result[1], vec_result[2], vec_result[3]);
    
    printf("Dot product: %.1f\n", dot_product_sse(vec1, vec2, 4));         // Should print: 70.0 (1*5 + 2*6 + 3*7 + 4*8)
    
    printf("Fast inv sqrt(4.0) = %.5f\n", fast_inv_sqrt_sse(4.0f));        // Should print: ~0.50000
    printf("Expected 1/sqrt(4.0) = %.5f\n", 1.0f / sqrtf(4.0f));          // Should print: 0.50000
    
    return 0;
}