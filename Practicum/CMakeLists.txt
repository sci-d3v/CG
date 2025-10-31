set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include utility functions
include(cmake/FileUtils.cmake)

add_subdirectory(P2-Geometric_primitives)
add_subdirectory(P3-Texture)
add_subdirectory(P4-3D_model)
add_subdirectory(P5-Geometry_pattern)
