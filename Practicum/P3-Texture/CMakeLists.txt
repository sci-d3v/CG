cmake_minimum_required(VERSION 3.20)
project(CG_P3)

set(CMAKE_CXX_STANDARD 17)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
configure_file(${CMAKE_SOURCE_DIR}/shaders/vertex.glsl ${CMAKE_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/fragment.glsl ${CMAKE_BINARY_DIR}/shaders/fragment.glsl COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/textures)
configure_file(${CMAKE_SOURCE_DIR}/textures/window_star.png ${CMAKE_BINARY_DIR}/textures/window_star.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/textures/planets.png ${CMAKE_BINARY_DIR}/textures/planets.png COPYONLY)

add_executable(CG_P3 main.cpp)

if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    target_link_libraries(CG_P3 m SOIL glfw GLEW GL)
else()
    message(STATUS "Windows")
    target_link_libraries(CG_P3 m SOIL glu32 opengl32 glfw3 glew32)
endif()


