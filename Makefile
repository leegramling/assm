# Makefile for x64 Assembly Tutorial Suite

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -g -Wall -Wextra -O0
CXXFLAGS = -g -Wall -Wextra -O0
LDFLAGS = 

# Tutorial executables
C_TUTORIALS = tutorial1 tutorial2 tutorial3 tutorial4 tutorial6 tutorial7 tutorial8 tutorial9
CPP_TUTORIALS = tutorial10 tutorial12 debug_example
OPTIMIZATION_TUTORIALS = tutorial11_debug tutorial11_optimized

ALL_TUTORIALS = $(C_TUTORIALS) $(CPP_TUTORIALS) $(OPTIMIZATION_TUTORIALS)

# Default target
all: $(ALL_TUTORIALS)

# Pattern rules for C tutorials
tutorial1: tutorial1_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial2: tutorial2_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial3: tutorial3_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial4: tutorial4_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial6: tutorial6_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial7: tutorial7_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial8: tutorial8_complete.c
	$(CC) $(CFLAGS) -o $@ $<

tutorial9: tutorial9_complete.c
	$(CC) $(CFLAGS) -o $@ $<

# C++ tutorials
tutorial10: tutorial10_complete.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

tutorial12: tutorial12.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Tutorial 5 - Debug example
library.o: library.cpp
	$(CXX) -c -O0 -o $@ $<

debug_example: main.cpp library.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Tutorial 11 - Optimization comparison
tutorial11_debug: tutorial11.cpp
	$(CXX) -O0 -g -Wall -o $@ $<

tutorial11_optimized: tutorial11.cpp
	$(CXX) -O2 -g -Wall -o $@ $<

# Run all tutorials
test: all
	@echo "=== Running all tutorials ==="
	@echo "\n--- Tutorial 1: Basic registers ---"
	@./tutorial1
	@echo "\n--- Tutorial 2: Arithmetic operations ---"
	@./tutorial2
	@echo "\n--- Tutorial 3: Control flow ---"
	@./tutorial3
	@echo "\n--- Tutorial 4: Function calls ---"
	@./tutorial4
	@echo "\n--- Tutorial 5: Debug example ---"
	@./debug_example
	@echo "\n--- Tutorial 6: String operations ---"
	@./tutorial6
	@echo "\n--- Tutorial 7: Array processing ---"
	@./tutorial7
	@echo "\n--- Tutorial 8: Bitwise operations ---"
	@./tutorial8
	@echo "\n--- Tutorial 9: SSE operations ---"
	@./tutorial9
	@echo "\n--- Tutorial 10: Mixed C++/Assembly ---"
	@./tutorial10
	@echo "\n--- Tutorial 11: Debug build ---"
	@./tutorial11_debug
	@echo "\n--- Tutorial 11: Optimized build ---"
	@./tutorial11_optimized
	@echo "\n--- Tutorial 12: C++ object layout ---"
	@./tutorial12

# Generate assembly listings
assembly: 
	@echo "=== Generating assembly listings ==="
	$(CC) -S -masm=intel tutorial1_complete.c -o tutorial1.s
	$(CC) -S -masm=intel tutorial2_complete.c -o tutorial2.s
	$(CC) -S -masm=intel tutorial3_complete.c -o tutorial3.s
	$(CC) -S -masm=intel tutorial4_complete.c -o tutorial4.s
	$(CC) -S -masm=intel tutorial6_complete.c -o tutorial6.s
	$(CC) -S -masm=intel tutorial7_complete.c -o tutorial7.s
	$(CC) -S -masm=intel tutorial8_complete.c -o tutorial8.s
	$(CC) -S -masm=intel tutorial9_complete.c -o tutorial9.s
	$(CXX) -S -masm=intel tutorial10_complete.cpp -o tutorial10.s
	$(CXX) -S -masm=intel -O0 tutorial11.cpp -o tutorial11_O0.s
	$(CXX) -S -masm=intel -O2 tutorial11.cpp -o tutorial11_O2.s
	$(CXX) -S -masm=intel tutorial12.cpp -o tutorial12.s
	@echo "Assembly files generated with Intel syntax"

# Clean build artifacts
clean:
	rm -f $(ALL_TUTORIALS) *.o *.s

# Clean everything including generated executables
distclean: clean
	rm -f core *.dSYM

# Help message
help:
	@echo "x64 Assembly Tutorial Makefile"
	@echo "=============================="
	@echo "Targets:"
	@echo "  all      - Build all tutorials (default)"
	@echo "  test     - Build and run all tutorials"
	@echo "  assembly - Generate assembly listings (Intel syntax)"
	@echo "  clean    - Remove build artifacts"
	@echo "  distclean- Remove all generated files"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Individual tutorials:"
	@echo "  tutorial1-12 - Build specific tutorial"
	@echo "  debug_example - Build Tutorial 5 debug example"
	@echo ""
	@echo "Example usage:"
	@echo "  make all        # Build everything"
	@echo "  make test       # Build and run all tutorials"
	@echo "  make tutorial1  # Build only tutorial 1"
	@echo "  make assembly   # Generate assembly listings"

.PHONY: all test assembly clean distclean help