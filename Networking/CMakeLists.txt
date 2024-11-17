cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sockpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Networking/include)

add_subdirectory(sockpp)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/Networking/*.hpp")

add_library(durak_networking STATIC ${SOURCES})
target_link_libraries(durak_networking sockpp)