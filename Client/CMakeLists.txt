cmake_minimum_required(VERSION 3.10)
project(DurakClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#GLAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include)
add_library(glad STATIC libs/glad/src/glad.c)

#GLFW
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include)
add_subdirectory(libs/glfw)
include_directories(${GLFW_INCLUDE_DIRS})

#STB
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/stb)

#FreeType
find_package(Freetype REQUIRED)

#OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# If on Apple Silicon, use the Homebrew version of GLM
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        # M1 Mac (Apple Silicon)
        include_directories("/opt/homebrew/include")
    else()
        # Intel Mac
        include_directories("/usr/local/include")
    endif()
endif()

#Networking
include_directories(${CMAKE_SOURCE_DIR}/Networking/include)
include_directories(${CMAKE_SOURCE_DIR}/Networking/rapidjson/include)

# Add executable
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(DurakClient ${SOURCES})

# Link libraries
target_link_libraries(DurakClient glad glfw ${OPENGL_LIBRARIES} Freetype::Freetype durak_networking)