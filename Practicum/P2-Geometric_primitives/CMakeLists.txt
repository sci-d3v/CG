cmake_minimum_required(VERSION 3.20)
project(cg_geometric_primitives)

copy_files(shaders *)
get_file_list(shaders glsl GLSL_SOURCE_FILES)

add_custom_target(CompileShaders DEPENDS ${GLSL_SOURCE_FILES})
# Ensure shaders are compiled before the executable is run

add_executable(cg_geometric_primitives geometric_primitives.cpp)
add_dependencies(cg_geometric_primitives CompileShaders)

    
if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    target_link_libraries(cg_geometric_primitives m glfw GLEW GL SOIL)
else()
    message(STATUS "Windows")
    target_link_libraries(cg_geometric_primitives m SOIL glu32 opengl32 glfw3 glew32)
endif()
