cmake_minimum_required(VERSION 3.10)
project(test_stuff)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# include_directories(${CMAKE_SOURCE_DIR}/Server)


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
add_executable(game_tests test_helpers.cpp test_cardEvent.cpp
               ${CMAKE_SOURCE_DIR}/Server/gamehelper.cpp
               ${CMAKE_SOURCE_DIR}/Server/game.cpp)

# include headers
target_include_directories(game_tests
  PRIVATE
  ${CMAKE_SOURCE_DIR}        # project root headers
  ${CMAKE_SOURCE_DIR}/Server # server headers
)
             
# target_link_libraries(hello_world_test gtest_main)
target_link_libraries(game_tests gtest_main)

# Register tests
# add_test(NAME DurakTestHelpers COMMAND game_tests)


include(GoogleTest)
gtest_discover_tests(game_tests)


