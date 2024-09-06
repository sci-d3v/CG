cmake_minimum_required(VERSION 3.26)
project(time_to_triangle)
set(CMAKE_CXX_STANDARD 17)


# project build
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
# find_package(glm REQUIRED)
add_executable(time_to_triangle main.cpp)
target_link_libraries(time_to_triangle PRIVATE
        glfw3
        Vulkan::Vulkan
)

# Shader compile
find_program(GLSL_VALIDATOR "glslangValidator" REQUIRED)
message(STATUS ${GLSL_VALIDATOR})

set(GLSL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_base.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_base.vert)

set(SPIRV_OUTPUT_FILES "")
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    list(APPEND SPIRV_OUTPUT_FILES ${SPIRV})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            VERBATIM
    )
endforeach()

add_custom_target(CompileShaders DEPENDS ${SPIRV_OUTPUT_FILES})

# Ensure shaders are compiled before the executable is run
add_dependencies(time_to_triangle CompileShaders)
