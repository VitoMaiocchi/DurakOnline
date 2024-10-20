cmake_minimum_required(VERSION 3.10)

project(DurakNetworking)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/sockpp/include)
include_directories(${PROJECT_SOURCE_DIR}/rapidjson/include)

add_subdirectory(sockpp)

set(SERVER_SRC server.cpp message.cpp network.cpp)
set(CLIENT_SRC client.cpp message.cpp network.cpp)

#server
add_executable(server ${SERVER_SRC})
target_link_libraries(server sockpp)

#client
add_executable(client ${CLIENT_SRC})
target_link_libraries(client sockpp)