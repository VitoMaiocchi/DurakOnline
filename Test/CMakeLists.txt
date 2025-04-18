cmake_minimum_required(VERSION 3.10)
project(test_stuff)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/Networking/include)
include_directories(${CMAKE_SOURCE_DIR}/Networking/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/Server/include)

# server
# add_executable(test_server2
#     server2.cpp
#     ${CMAKE_SOURCE_DIR}/Server/src/game.cpp
#     ${CMAKE_SOURCE_DIR}/Server/src/battle.cpp
#     ${CMAKE_SOURCE_DIR}/Server/src/card_manager.cpp
# )
add_executable(test_server server.cpp)
# add_executable(test_server server.cpp)
# target_link_libraries(test_server2 durak_networking)
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
# add_executable(hello_world_test ${CMAKE_SOURCE_DIR}/Test/hello_world_test.cpp)

# Test for Battle
add_executable(test_battle test_battle.cpp
               ${CMAKE_SOURCE_DIR}/Server/src/battle.cpp
               ${CMAKE_SOURCE_DIR}/Server/src/card_manager.cpp)

# target_link_libraries(hello_world_test gtest_main)
target_link_libraries(test_battle gtest_main durak_networking)

# Test for Game
add_executable(test_game
    test_game.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/game.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/battle.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/card_manager.cpp
)
target_link_libraries(test_game gtest_main durak_networking)

# Test for Roles
add_executable(test_roles 
    test_roles.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/game.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/battle.cpp
    ${CMAKE_SOURCE_DIR}/Server/src/card_manager.cpp
    )
target_link_libraries(test_roles gtest_main durak_networking)
# add_test(NAME hello_world_test COMMAND hello_world_test)

# Register tests
add_test(NAME DurakTests COMMAND test_battle)
add_test(NAME DurakGameTests COMMAND test_game)
add_test(NAME DurakRoleTests COMMAND test_roles)

include(GoogleTest)
gtest_discover_tests(test_battle)
gtest_discover_tests(test_game)
gtest_discover_tests(test_roles)
