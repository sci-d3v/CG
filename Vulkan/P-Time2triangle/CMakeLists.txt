cmake_minimum_required(VERSION 3.26)
project(time2triangle)
set(CMAKE_CXX_STANDARD 17)


# project build
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
# find_package(glm REQUIRED)
add_executable(time2triangle main.cpp)
target_link_libraries(time2triangle PRIVATE
        glfw3
        Vulkan::Vulkan
)

# shader compile
add_executable(glslang::validator IMPORTED)
find_program(GLSL_VALIDATOR "glslangValidator" REQUIRED)
message(STATUS ${GLSL_VALIDATOR})
set_property(TARGET glslang::validator PROPERTY IMPORTED_LOCATION "${GLSL_VALIDATOR}")

set(GLSL_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_base.frag
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader_base.vert)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
            TARGET time2triangle PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders/"
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            VERBATIM
    )
endforeach(GLSL)

