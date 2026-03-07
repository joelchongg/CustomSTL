#include <gtest/gtest.h>
#include "CustomSTL/unique_ptr.hpp"

// Test if operator bool() & default constructor works correctly
TEST(UniquePtrTest, DefaultConstructor) {
    CustomSTL::unique_ptr<int> p;
    EXPECT_FALSE(p);
    EXPECT_EQ(p.get(), nullptr);
}

// Test if pointer constructor, operator bool() & operator* work correctly
TEST(UniquePtrTest, PointerConstructorAndAccess) {
    CustomSTL::unique_ptr<int> p { new int(42) };
    ASSERT_TRUE(p);
    EXPECT_EQ(*p, 42);
    *p = 100;
    EXPECT_EQ(*p, 100);
}

// Test if move constructor works correctly
TEST(UniquePtrTest, MoveConstructor) {
    CustomSTL::unique_ptr<int> p1 { new int(55) };
    CustomSTL::unique_ptr<int> p2 { std::move(p1) };    
    EXPECT_EQ(*p2, 55);
    EXPECT_EQ(p1.get(), nullptr);
}

// Test if move assignment operator works correctly
TEST(UniquePtrTest, MoveAssignment) {
    CustomSTL::unique_ptr<int> p1 { new int(77) };
    CustomSTL::unique_ptr<int> p2;
    p2 = std::move(p1);
    EXPECT_EQ(*p2, 77);
    EXPECT_EQ(p1.get(), nullptr);
}

// Test if reset() and release() work correctly
TEST(UniquePtrTest, ResetAndRelease) {
    CustomSTL::unique_ptr<int> p { new int(9) };
    EXPECT_TRUE(p);
    p.reset();
    EXPECT_FALSE(p);
    p.reset(new int(10));
    EXPECT_EQ(*p, 10);
    int* raw = p.release();
    EXPECT_EQ(*raw, 10);
    EXPECT_FALSE(p);
    delete raw;
}