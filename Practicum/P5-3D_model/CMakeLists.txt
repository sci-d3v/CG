cmake_minimum_required(VERSION 3.14)
set(_TARGET 3d_model)
project(${_TARGET})

set(CMAKE_CXX_STANDARD 17)

copy_files(shaders *)
copy_files(assets *)


add_executable(${_TARGET} src/main.cpp src/mesh.cpp src/model.cpp src/shader.cpp)

if(UNIX AND NOT APPLE)
    message(STATUS "Linux")
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)
    # Подключаем модуль для загрузки исходников
    include(FetchContent)
    # Загружаем SOIL
    FetchContent_Declare(
    SOIL
    GIT_REPOSITORY https://github.com/littlstar/soil.git
    GIT_TAG        master  # можно указать конкретную версию или ветку
    )
    FetchContent_MakeAvailable(SOIL)
    file(GLOB_RECURSE SOIL_LIB_SRCS ${soil_SOURCE_DIR}/src/*.c)
    # Создаем статическую библиотеку SOIL
    add_library(
        SOIL_LIB STATIC 
        ${SOIL_LIB_SRCS}
    )
    # Указываем путь к заголовочным файлам SOIL
    target_include_directories(SOIL_LIB PUBLIC ${soil_SOURCE_DIR}/include)
    target_link_libraries(${_TARGET} m glfw GLEW GL SOIL_LIB assimp)
else()
    message(STATUS "Windows")
    target_link_libraries(${_TARGET} m glu32 SOIL opengl32 glfw3 glew32 assimp)
endif()
