cmake_minimum_required(VERSION 3.10)
project(test_stuff)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/Server)



# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Test for Helpers
add_executable(test_helpers test_helpers.cpp
               ${CMAKE_SOURCE_DIR}/Server/gamehelper.cpp)

# target_link_libraries(hello_world_test gtest_main)
target_link_libraries(test_helpers gtest_main)

# Register tests
add_test(NAME DurakTestHelpers COMMAND test_helpers)


include(GoogleTest)
gtest_discover_tests(test_helpers)


