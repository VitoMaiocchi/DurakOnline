// CMakeLists.txt:
// ```
// cmake_minimum_required(VERSION 3.10)
// project(project-name)

// set(CMAKE_CXX_STANDARD 11)

// add_executable(hello_world_test hello_world_test.cpp)
// target_link_libraries(hello_world_test gtest gtest_main)
// ```

// hello_world_test.cpp:
// ```
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
