cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/Networking/include)
include_directories(${CMAKE_SOURCE_DIR}/Networking/rapidjson/include)

#server
add_executable(test_server server.cpp)
target_link_libraries(test_server durak_networking)

#client
add_executable(test_client client.cpp)
target_link_libraries(test_client durak_networking)