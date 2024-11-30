cmake_minimum_required(VERSION 3.14)

project(CG_P6)

set(CMAKE_CXX_STANDARD 20)
# set (CMAKE_CXX_FLAGS -O0)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")


include(CMakePrintHelpers)

function(COPY_FILES _DIRECTORY EXTENSION)
    #    message(STATUS "Parameters = ${ARGV}")
    message(STATUS "Copying from \"${_DIRECTORY}\" directory with \"${EXTENSION}\" extension")
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${_DIRECTORY})

    file(GLOB_RECURSE _PATHS "${CMAKE_SOURCE_DIR}/${_DIRECTORY}/*.${EXTENSION}")
    set(_NUMBER 1)
    message(STATUS "List of Files:")
    foreach(_PATH ${_PATHS})
        get_filename_component(_NAME ${_PATH} NAME)
        configure_file(${CMAKE_SOURCE_DIR}/${_DIRECTORY}/${_NAME} ${CMAKE_BINARY_DIR}/${_DIRECTORY}/${_NAME} COPYONLY)
        message(STATUS "${_NUMBER}. Copying \"${_NAME}\"")
        math(EXPR _NUMBER "${_NUMBER}+1")
    endforeach()
endfunction()

copy_files(shaders glsl)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(CG_P6    src/main.cpp 
                        src/shader.cpp 
                        src/scene.cpp
                        src/icosahedron.cpp 
                        )

if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    target_link_libraries(CG_P6 m glfw GLEW GL)
else()
    message(STATUS "Windows")
    target_link_libraries(CG_P6 m glu32 opengl32 glfw3 glew32)
endif()

