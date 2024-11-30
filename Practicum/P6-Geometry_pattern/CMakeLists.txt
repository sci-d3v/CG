cmake_minimum_required(VERSION 3.14)

project(geometry_pattern)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Вывод информации о текущем типе сборки
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


function(copy_files SOURCE EXTENSION)
    message(STATUS "Copying from \"${SOURCE}\" directory with \"${EXTENSION}\" extension")
    file(GLOB_RECURSE FILES RELATIVE ${CMAKE_SOURCE_DIR}/${SOURCE} ${CMAKE_SOURCE_DIR}/${SOURCE}/*.${EXTENSION})
    set(_FILE_NUMBER 0)
    set(_DIR_NUMBER 0)
    message(STATUS "List of Copied Files (and created necessary directories):")
    foreach(FILE ${FILES})
        set(SOURCE_FILE ${CMAKE_SOURCE_DIR}/${SOURCE}/${FILE})
        set(DEST_FILE ${CMAKE_BINARY_DIR}/${SOURCE}/${FILE})
        get_filename_component(DIR ${DEST_FILE} DIRECTORY)
        if(NOT EXISTS ${DIR})
            math(EXPR _DIR_NUMBER "${_DIR_NUMBER}+1")
            message(STATUS "${_DIR_NUMBER}. Making \"${DIR}\" directory")
            file(MAKE_DIRECTORY ${DIR})
        endif()
        # if(NOT EXISTS ${DEST_FILE})
            math(EXPR _FILE_NUMBER "${_FILE_NUMBER}+1")
            message(STATUS "${_FILE_NUMBER}. Copying \"${SOURCE}/${FILE}\" file")
            configure_file(${SOURCE_FILE} ${DEST_FILE} COPYONLY)
        # endif()
    endforeach()

endfunction()

copy_files(shaders glsl)

include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

if(UNIX AND NOT APPLE)
    message(STATUS "Linux platform")
    find_package(Freetype REQUIRED)
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} m glfw GLEW GL ${FREETYPE_LIBRARIES} SOIL assimp)
elseif(WIN32)
    message(STATUS "Windows platform")
    target_link_libraries(${CMAKE_PROJECT_NAME} m glu32 opengl32 glfw3 glew32 SOIL assimp)
else()
    message(SEND_ERROR "The project supports only Linux and Windows platforms")
endif()
