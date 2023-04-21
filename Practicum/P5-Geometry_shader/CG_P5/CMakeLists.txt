cmake_minimum_required(VERSION 3.0)
project(CG_P5)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
configure_file(${CMAKE_SOURCE_DIR}/shaders/vertex.glsl ${CMAKE_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/geometry.glsl ${CMAKE_BINARY_DIR}/shaders/geometry.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/fragment.glsl ${CMAKE_BINARY_DIR}/shaders/fragment.glsl COPYONLY)

add_executable(CG_P5 main.cpp)

if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    target_link_libraries(CG_P5 m glfw GLEW GL)
else()
    message(STATUS "Windows")
    target_link_libraries(CG_P5 m glu32 opengl32 glfw3 glew32)
endif()
