cmake_minimum_required(VERSION 3.10)
project(DurakServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Networking
include_directories(${CMAKE_SOURCE_DIR}/Networking/include)
include_directories(${CMAKE_SOURCE_DIR}/Networking/rapidjson/include)


# Define sources for both the library and the executable
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

#"${CMAKE_CURRENT_SOURCE_DIR}/Networking/src/*.cpp"

# Create a static library
#add_library(DurakLib STATIC ${SOURCES})

# Add executable with a different name and link it to the library
add_executable(DurakServer ${SOURCES})
target_link_libraries(DurakServer durak_networking)
