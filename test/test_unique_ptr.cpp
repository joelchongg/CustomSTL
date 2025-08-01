#include <iostream>
#include "customSTL/unique_ptr.hpp"

struct Test {
    int x = 42;
    ~Test() { std::cout << "Test destroyed\n"; }
};

int main() {
    customSTL::unique_ptr<Test> ptr1(new Test);
    if (ptr1) {
        std::cout << ptr1->x << '\n';
    }

    customSTL::unique_ptr<Test> ptr2(std::move(ptr1));
    if (!ptr1) {
        std::cout << "ptr1 is null\n";
    }
}