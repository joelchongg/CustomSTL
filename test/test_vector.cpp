#include <gtest/gtest.h>
#include "CustomSTL/vector.hpp"

TEST(VectorTest, Realloc) {
    CustomSTL::vector<int> vec;
    EXPECT_EQ(vec.capacity(), 0);

    vec.push_back(1);
    EXPECT_EQ(vec.capacity(), 1);


    vec.push_back(1);
    EXPECT_EQ(vec.capacity(), 2);

    vec.push_back(1);
    EXPECT_EQ(vec.capacity(), 4);
}