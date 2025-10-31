get_filename_component(CURRENT_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(TOLOWER "${CURRENT_PROJECT}" CURRENT_PROJECT)
project(${CURRENT_PROJECT})

COPY_FILES_TO_BUILD(shaders *)
COPY_FILES_TO_BUILD(assets *)

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${CURRENT_PROJECT} ${SRCS})

GET_FILES_BY_EXTENSION(shaders glsl GLSL_SOURCE_FILES)
add_custom_target(${CURRENT_PROJECT}_compile_shaders DEPENDS ${GLSL_SOURCE_FILES})
add_dependencies(${CURRENT_PROJECT} ${CURRENT_PROJECT}_compile_shaders)

target_include_directories(${CURRENT_PROJECT} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ..  # adds practicum/ to the include path
)

target_link_libraries(${CURRENT_PROJECT} m SOIL assimp)
if(UNIX AND NOT APPLE)
    message(STATUS "Linux platform")
    target_link_libraries(${CURRENT_PROJECT} glfw GLEW GL)
elseif(WIN32)
    message(STATUS "Windows platform")
    target_link_libraries(${CURRENT_PROJECT} glu32 opengl32 glfw3 glew32)
else()
    message(SEND_ERROR "The project supports only Linux and Windows platforms")
endif()
