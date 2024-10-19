cmake_minimum_required(VERSION 3.26)
project(vulkan_context)
set(CMAKE_CXX_STANDARD 17)

# project build
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
add_executable(vulkan_context src/main.cpp)
target_link_libraries(vulkan_context PRIVATE glfw3 Vulkan::Vulkan)
