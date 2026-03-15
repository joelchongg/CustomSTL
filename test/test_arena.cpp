#include <gtest/gtest.h>
#include "CustomSTL/arena.hpp"

// Test allocation, making sure the correct amount of bytes is allocated and
// aligned, while ensuring that std::bad_alloc is thrown when all memory is used
TEST(ArenaTest, Allocation) {
    CustomSTL::Arena arena(64);

    // no memory allocated, remaining size = 64 bytes
    EXPECT_EQ(arena.remaining(), 64uz);
    
    // allocate for an integer, remaining size = 60 bytes
    arena.allocate(4, alignof(int));
    EXPECT_EQ(arena.remaining(), 60uz);

    // allocate a char and then an integer
    // remaining size should be 52 due to padding for the integer
    arena.allocate(1, alignof(char));
    EXPECT_EQ(arena.remaining(), 59uz);
    arena.allocate(4, alignof(int));
    EXPECT_EQ(arena.remaining(), 52uz);

    arena.allocate(8, alignof(double));
    EXPECT_EQ(arena.remaining(), 40uz);

    arena.allocate(40, alignof(double));
    EXPECT_EQ(arena.remaining(), 0uz);

    // arena fully used up
    EXPECT_THROW(arena.allocate(1, alignof(char)), std::bad_alloc);
}

// Test using arena.construct() instead of arena.allocate as per previous test

TEST(ArenaTest, DirectConstruction) {
    CustomSTL::Arena arena(64);

    // no memory allocated, remaining size = 64 bytes
    EXPECT_EQ(arena.remaining(), 64uz);
    
    // allocate for an integer, remaining size = 60 bytes
    arena.construct<int>(1);
    EXPECT_EQ(arena.remaining(), 60uz);

    // allocate a char and then an integer
    // remaining size should be 52 due to padding for the integer
    arena.construct<char>('x');
    EXPECT_EQ(arena.remaining(), 59uz);
    arena.construct<int>(2);
    EXPECT_EQ(arena.remaining(), 52uz);

    arena.construct<double>(3.0);
    EXPECT_EQ(arena.remaining(), 40uz);

    // use up all remaining bytes
    for (int i = 0; i < 5; ++i) {
        arena.construct<double>(4.0);
    }
    EXPECT_EQ(arena.remaining(), 0uz);

    // arena fully used up
    EXPECT_THROW(arena.construct<char>('y'), std::bad_alloc);
}