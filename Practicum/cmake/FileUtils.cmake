# File utility functions for the Practicum project

function(COPY_FILES_TO_BUILD _DIRECTORY _EXTENSION)
    # Copy files with specified extension from source directory to build directory
    message(STATUS "Copying from \"${_DIRECTORY}\" directory with \"${_EXTENSION}\" extension")
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_DIRECTORY})

    file(GLOB_RECURSE _PATHS "${CMAKE_CURRENT_SOURCE_DIR}/${_DIRECTORY}/*.${_EXTENSION}")
    set(_NUMBER 1)
    message(STATUS "List of Files:")
    foreach(_PATH ${_PATHS})
        get_filename_component(_NAME ${_PATH} NAME)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_DIRECTORY}/${_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${_DIRECTORY}/${_NAME} COPYONLY)
        message(STATUS "${_NUMBER}. Copying \"${_NAME}\"")
        math(EXPR _NUMBER "${_NUMBER}+1")
    endforeach()
endfunction()

function(GET_FILES_BY_EXTENSION DIRECTORY EXTENSION OUTPUT_VAR)
    # Get list of files with specified extension from directory
    file(GLOB_RECURSE _FILES "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.${EXTENSION}")
    set(${OUTPUT_VAR} ${_FILES} PARENT_SCOPE)
endfunction()
