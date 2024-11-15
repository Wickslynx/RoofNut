cmake_minimum_required(VERSION 3.14)
project(RoofNut)

# Set C and/or C++ standard
set(CMAKE_C_STANDARD 99)  # For Nuklear compatibility, adjust as needed

# Define the source files for your library
set(SOURCE_FILES 
    src/application.c
    src/input.c
    src/Utilites.c
)

# Add the RoofNut static library
add_library(RoofNut STATIC ${SOURCE_FILES})

# Use FetchContent for GLFW
include(FetchContent)

# GLFW setup
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland support")
set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 support")
FetchContent_MakeAvailable(glfw)

# Find Vulkan and OpenGL packages
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

# Link libraries to RoofNut
target_link_libraries(RoofNut PRIVATE
    glfw
    Vulkan::Vulkan
    OpenGL::GL
)

# Add Nuklear manually since it's not built as a library
file(GLOB NUKLEAR_SOURCES ${CMAKE_SOURCE_DIR}/include/librarys/Nuklear/*.c)
target_sources(RoofNut PRIVATE ${NUKLEAR_SOURCES})

# Include directories for RoofNut and Nuklear
target_include_directories(RoofNut PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/librarys/Nuklear
)

# Ensure that Vulkan and OpenGL include directories are linked properly
target_include_directories(RoofNut PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)

# Set output directories for compiled library
set_target_properties(RoofNut PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install the library and headers
install(TARGETS RoofNut DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
