cmake_minimum_required(VERSION 3.10)
project(DurakClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#GLAD
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
add_library(glad STATIC glad/src/glad.c)

#GLFW
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
add_subdirectory(glfw)
include_directories(${GLFW_INCLUDE_DIRS})

#STB
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stb)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/freetype/include)
#add_subdirectory(freetype)
find_package(Freetype REQUIRED)

#OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)


# Add executable
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(DurakClient ${SOURCES})

# Link libraries
target_link_libraries(DurakClient glad glfw ${OPENGL_LIBRARIES} Freetype::Freetype)