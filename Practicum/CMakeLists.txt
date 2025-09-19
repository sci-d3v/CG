function(COPY_FILES _DIRECTORY _EXTENSION)
#    message(STATUS "Parameters = ${ARGV}")
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



function(get_file_list DIRECTORY EXTENSION OUTPUT_VAR)
    file(GLOB_RECURSE _SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.${EXTENSION}")
    set(${OUTPUT_VAR} ${_SHADER_FILES} PARENT_SCOPE)
endfunction()

add_subdirectory(P2-Geometric_primitives)
