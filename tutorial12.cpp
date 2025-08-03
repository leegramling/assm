// tutorial12.cpp - Understanding C++ object layout and vtables
#include <iostream>
#include <memory>

// Simple base class to demonstrate object layout
class SimpleClass {
private:
    int value1;
    int value2;
    
public:
    SimpleClass(int v1, int v2) : value1(v1), value2(v2) {}
    
    void print_values() const {
        std::cout << "Values: " << value1 << ", " << value2 << std::endl;
    }
    
    int get_sum() const { return value1 + value2; }
};

// Base class with virtual functions
class Base {
private:
    int base_data;
    
public:
    Base(int data) : base_data(data) {}
    
    virtual ~Base() = default;
    
    virtual void virtual_method() const {
        std::cout << "Base::virtual_method(), data: " << base_data << std::endl;
    }
    
    virtual int pure_virtual_method() const = 0;
    
    void non_virtual_method() const {
        std::cout << "Base::non_virtual_method()" << std::endl;
    }
    
    int get_base_data() const { return base_data; }
};

// Derived class
class Derived : public Base {
private:
    int derived_data;
    
public:
    Derived(int base_val, int derived_val) 
        : Base(base_val), derived_data(derived_val) {}
    
    void virtual_method() const override {
        std::cout << "Derived::virtual_method(), data: " << derived_data << std::endl;
    }
    
    int pure_virtual_method() const override {
        return get_base_data() + derived_data;
    }
    
    void derived_only_method() const {
        std::cout << "Derived::derived_only_method()" << std::endl;
    }
};

// Multiple inheritance example
class Interface1 {
public:
    virtual ~Interface1() = default;
    virtual void interface1_method() const = 0;
};

class Interface2 {
public:
    virtual ~Interface2() = default;
    virtual void interface2_method() const = 0;
};

class MultipleInheritance : public Interface1, public Interface2 {
private:
    int data;
    
public:
    MultipleInheritance(int d) : data(d) {}
    
    void interface1_method() const override {
        std::cout << "MultipleInheritance::interface1_method(), data: " << data << std::endl;
    }
    
    void interface2_method() const override {
        std::cout << "MultipleInheritance::interface2_method(), data: " << data << std::endl;
    }
};

// Function to demonstrate polymorphism
void call_virtual_method(const Base& obj) {
    // This call will use vtable lookup
    obj.virtual_method();
    
    // This call is direct (no vtable)
    obj.non_virtual_method();
}

// Function to analyze object sizes and layouts
void analyze_object_layouts() {
    std::cout << "=== Object Size Analysis ===" << std::endl;
    std::cout << "sizeof(SimpleClass): " << sizeof(SimpleClass) << " bytes" << std::endl;
    std::cout << "sizeof(Base): " << sizeof(Base) << " bytes" << std::endl;
    std::cout << "sizeof(Derived): " << sizeof(Derived) << " bytes" << std::endl;
    std::cout << "sizeof(Interface1): " << sizeof(Interface1) << " bytes" << std::endl;
    std::cout << "sizeof(MultipleInheritance): " << sizeof(MultipleInheritance) << " bytes" << std::endl;
    std::cout << std::endl;
    
    // Create objects to analyze memory layout
    SimpleClass simple(10, 20);
    Derived derived(100, 200);
    MultipleInheritance multi(300);
    
    std::cout << "=== Memory Addresses ===" << std::endl;
    std::cout << "SimpleClass object address: " << &simple << std::endl;
    std::cout << "Derived object address: " << &derived << std::endl;
    
    // Cast to base to see pointer adjustment
    Base* base_ptr = &derived;
    std::cout << "Derived as Base* address: " << base_ptr << std::endl;
    
    // Multiple inheritance pointer analysis
    Interface1* if1_ptr = &multi;
    Interface2* if2_ptr = &multi;
    std::cout << "MultipleInheritance as Interface1*: " << if1_ptr << std::endl;
    std::cout << "MultipleInheritance as Interface2*: " << if2_ptr << std::endl;
    std::cout << "MultipleInheritance object address: " << &multi << std::endl;
    std::cout << std::endl;
}

// Function to demonstrate vtable usage
void demonstrate_vtable_calls() {
    std::cout << "=== Virtual Function Calls ===" << std::endl;
    
    std::unique_ptr<Base> base_ptr = std::make_unique<Derived>(42, 84);
    
    // Virtual function call - uses vtable
    base_ptr->virtual_method();
    
    // Pure virtual function call - uses vtable
    int result = base_ptr->pure_virtual_method();
    std::cout << "Pure virtual result: " << result << std::endl;
    
    // Non-virtual function call - direct call
    base_ptr->non_virtual_method();
    
    std::cout << std::endl;
}

// Function to show polymorphic behavior
void demonstrate_polymorphism() {
    std::cout << "=== Polymorphism Demo ===" << std::endl;
    
    Derived derived1(1, 2);
    Derived derived2(10, 20);
    
    // Both calls use the same function but different vtables
    call_virtual_method(derived1);
    call_virtual_method(derived2);
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== C++ Object Layout and VTables Tutorial ===" << std::endl;
    std::cout << std::endl;
    
    analyze_object_layouts();
    demonstrate_vtable_calls();
    demonstrate_polymorphism();
    
    std::cout << "=== Debugging Tips ===" << std::endl;
    std::cout << "1. Use 'info vtbl object' in GDB to examine vtables" << std::endl;
    std::cout << "2. Use 'print *object' to see object memory layout" << std::endl;
    std::cout << "3. Use 'print (Base*)&derived' to see pointer casting" << std::endl;
    std::cout << "4. Virtual calls: load vtable pointer, index into vtable, call function" << std::endl;
    std::cout << "5. Multiple inheritance may adjust 'this' pointer" << std::endl;
    std::cout << "6. Object size = data members + vtable pointer(s) + padding" << std::endl;
    
    return 0;
}