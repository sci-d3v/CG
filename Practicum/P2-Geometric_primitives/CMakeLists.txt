get_filename_component(CURRENT_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(TOLOWER "${CURRENT_PROJECT}" CURRENT_PROJECT)
project(${CURRENT_PROJECT})

COPY_FILES_TO_BUILD(shaders glsl)

# Get shader files for dependency tracking
GET_FILES_BY_EXTENSION(shaders glsl GLSL_SOURCE_FILES)

# Create custom target for shader compilation
add_custom_target(${CURRENT_PROJECT}_compile_shaders DEPENDS ${GLSL_SOURCE_FILES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create executable
add_executable(${CURRENT_PROJECT} geometric_primitives.cpp)
add_dependencies(${CURRENT_PROJECT} ${CURRENT_PROJECT}_compile_shaders)

# Platform-specific libraries
if(UNIX AND NOT APPLE)
    message(STATUS "Linux platform")
    target_link_libraries(${CURRENT_PROJECT} m glfw GLEW GL SOIL)
elseif(WIN32)
    message(STATUS "Windows platform")
    target_link_libraries(${CURRENT_PROJECT} m SOIL glu32 opengl32 glfw3 glew32)
else()
    message(SEND_ERROR "The project supports only Linux and Windows platforms")
endif()
