cmake_minimum_required(VERSION 3.24)
project(CG_)
set(CMAKE_CXX_STANDARD 20)

include(CMakePrintHelpers)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

function(COPY_FILES _DIRECTORY _SUBDIRECTORY _EXTENSION)
    message(STATUS "COPY_FILES: ${ARGV} function parameters")
    message(STATUS "Copying from \"${_DIRECTORY}\/${_SUBDIRECTORY}\" directory with \"${_EXTENSION}\" extension")
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${_SUBDIRECTORY})

    file(GLOB_RECURSE _PATHS "${CMAKE_SOURCE_DIR}/${_DIRECTORY}/${_SUBDIRECTORY}/*.${_EXTENSION}")
    message(STATUS "Copying \"${_PATHS}\" files")
    set(_NUMBER 1)
    message(STATUS "List of Files:")
    foreach(_PATH ${_PATHS})
        get_filename_component(_NAME ${_PATH} NAME)
        configure_file(${CMAKE_SOURCE_DIR}/${_DIRECTORY}/${_SUBDIRECTORY}/${_NAME} ${CMAKE_BINARY_DIR}/${_SUBDIRECTORY}/${_NAME} COPYONLY)
        message(STATUS "${_NUMBER}. Copying \"${_NAME}\"")
        math(EXPR _NUMBER "${_NUMBER}+1")
    endforeach()
endfunction()

function(BUILD_SOURCE _DIRECTORY)
    file(GLOB_RECURSE _SAMPLES_SRCS "${PROJECT_SOURCE_DIR}/samples/${_DIRECTORY}/*.cpp")
    message(STATUS "BUILD_SOURCE FROM ${_SAMPLES_SRCS}")
    file(GLOB_RECURSE _SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")
    message(STATUS "BUILD_SOURCE FORM ${_SRCS}")
    add_executable(CG ${_SRCS} ${_SAMPLES_SRCS})
endfunction()

function(BUILD_PROJECT _DIRECTORY)
    message(STATUS "BUILD_PROJECT: ${ARGV} function parameters")
    message(STATUS "Work directory is \"${_DIRECTORY}\"")

    include_directories(${CMAKE_SOURCE_DIR}/samples/${_DIRECTORY}/include)

    copy_files(samples/${_DIRECTORY} shaders glsl)

    build_source(${_DIRECTORY})

    if(UNIX AND NOT APPLE)
        message(STATUS "Linux")
        target_link_libraries(CG m glfw GLEW GL png)
    else()
        message(STATUS "Windows")
        target_link_libraries(CG m glu32 opengl32 glfw3 glew32 png)
    endif()

endfunction()

# build_project("icosahedron_tessellation")
# build_project("cube_tessellation")
# build_project("P1-test")
build_project("fractals")

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/images)
