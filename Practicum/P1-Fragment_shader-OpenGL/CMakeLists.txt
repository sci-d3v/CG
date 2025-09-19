# cmake_minimum_required(VERSION 3.14)
# set(CMAKE_CXX_STANDARD 17)

project(CG_Test)

copy_files(shaders *)

set(GLSL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.glsl
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl)

add_custom_target(CompileShaders DEPENDS ${GLSL_SOURCE_FILES})
# Ensure shaders are compiled before the executable is run

add_executable(CG_Test main.cpp)
add_dependencies(CG_Test CompileShaders)
    
if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    target_link_libraries(CG_Test m glfw GLEW GL)
else()
    message(STATUS "Windows")
    target_link_libraries(CG_Test m glu32 opengl32 glfw3 glew32)
endif()
