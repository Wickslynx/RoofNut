# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(RoofNut VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files for the project
file(GLOB SOURCES
    src/application.c
    src/input.c 
    src/Utilites.c
)

# Create the static library target for RoofNut
add_library(RoofNut STATIC ${SOURCES})

# Specify the paths to include directories for GLFW and other dependencies
include_directories("/usr/include/GLFW")
include_directories("/usr/local/include/GLFW")

# Add include directories for your own headers and Nuklear
target_include_directories(RoofNut PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/librarys/Nuklear
)

# Add the directory where the GLFW library files are located
link_directories("/usr/lib/x86_64-linux-gnu")
link_directories("/usr/local/lib")

# Find Vulkan package
find_package(Vulkan REQUIRED)

# Link the required libraries to the target
target_link_libraries(RoofNut PRIVATE glfw)
target_link_libraries(RoofNut PRIVATE Vulkan::Vulkan)
target_link_libraries(RoofNut PRIVATE m)  # Link the math library

# target_compile_definitions(RoofNut PRIVATE DEBUG)

# Installation targets
install(TARGETS RoofNut
    DESTINATION /usr/local/lib  # Install the library to /usr/local/lib
)

# Install header files to /usr/local/include
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION /usr/local/include   
)

