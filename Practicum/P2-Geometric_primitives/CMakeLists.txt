cmake_minimum_required(VERSION 3.20)
project(CG_P2)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
configure_file(${CMAKE_SOURCE_DIR}/shaders/vertex.glsl ${CMAKE_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/fragment.glsl ${CMAKE_BINARY_DIR}/shaders/fragment.glsl COPYONLY)

add_executable(CG_P2 main.cpp)

    
if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    target_link_libraries(CG_P2 m glfw GLEW GL SOIL)
else()
    message(STATUS "Windows")
    target_link_libraries(CG_P2 m SOIL glu32 opengl32 glfw3 glew32)
endif()
