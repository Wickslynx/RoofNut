# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(RoofNut VERSION 1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add source files for the project
file(GLOB SOURCES
    src/application.c
    src/input.c
    src/Utilities.c
)

file(GLOB HEADERS
    include/application.h
    include/input.h
    include/Utilities.h
    include/roofnut.h
)

# Create the library target
add_library(RoofNut STATIC ${SOURCES} ${HEADERS})

# Include directories for project and dependencies
target_include_directories(RoofNut PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/librarys/Nuklear
)

# Option to choose Vulkan or OpenGL
option(USE_VULKAN "Use Vulkan for rendering" OFF)
option(USE_OPENGL "Use OpenGL for rendering" OFF)

if(USE_VULKAN)
    message(STATUS "Using Vulkan for rendering")
    target_compile_definitions(RoofNut PRIVATE ROOFNUT_USE_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(RoofNut PRIVATE glfw Vulkan::Vulkan m)
elseif(USE_OPENGL)
    message(STATUS "Using OpenGL for rendering")
    target_compile_definitions(RoofNut PRIVATE ROOFNUT_USE_OPENGL)
    find_package(OpenGL REQUIRED)
    target_link_libraries(RoofNut PRIVATE glfw OpenGL::GL m)
else()
    message(FATAL_ERROR "No rendering API selected. Please specify -DUSE_VULKAN=ON or -DUSE_OPENGL=ON")
endif()

# Installation targets
install(TARGETS RoofNut
    DESTINATION /usr/local/lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)

# Ignore this, Its made for my testing (Wickslynx).
# add_executable(RoofNutTest test.c)

# Ignore this, made for my testing (Wickslynx)
# target_link_libraries(RoofNutTest PRIVATE RoofNut glfw Vulkan::Vulkan m)
