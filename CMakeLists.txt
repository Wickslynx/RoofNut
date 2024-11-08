cmake_minimum_required(VERSION 3.10)

project(RoofNut)

# Find Vulkan package (Requires that Vulkan is installed)
find_package(Vulkan REQUIRED)

set(GLFW3_DIR "/include/GLFW/g)
# Find GLFW3 (if it is installed in a non-standard location)
find_package(GLFW3 REQUIRED)

find_package(GLFW3 REQUIRED)

# Add the source files to the executable
add_executable(RoofNut 
    src/application.c 
    src/input.c 
    src/Utilites.c 
    src/image.c
)

# Specify include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link Vulkan to the executable
target_link_libraries(RoofNut Vulkan::Vulkan)
