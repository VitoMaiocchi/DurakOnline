cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/Networking/include)
include_directories(${CMAKE_SOURCE_DIR}/Networking/rapidjson/include)

# server
add_executable(test_server server.cpp)
target_link_libraries(test_server durak_networking)

# client
add_executable(test_client client.cpp)
target_link_libraries(test_client durak_networking)

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

# Add test executable
add_executable(hello_world_test ${CMAKE_SOURCE_DIR}/Test/hello_world_test.cpp)
target_link_libraries(hello_world_test gtest_main)
add_test(NAME hello_world_test COMMAND hello_world_test)

include(GoogleTest)
gtest_discover_tests(hello_world_test)

