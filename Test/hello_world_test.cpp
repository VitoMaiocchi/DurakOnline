#include <gtest/gtest.h>

// Test case 1
TEST(HelloWorldTest, Test1) {
    // Test code goes here
    ASSERT_TRUE(true);
}

// Test case 2
TEST(HelloWorldTest, Test2) {
    // Test code goes here
    ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
