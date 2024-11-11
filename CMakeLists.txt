cmake_minimum_required(VERSION 3.14)
project(RoofNut)

# Set C++ standard if needed, e.g., for C++ projects.
# set(CMAKE_CXX_STANDARD 17)

# Set the source files for the library
set(SOURCE_FILES 
    src/application.c
    src/input.c
    src/Utilites.c
    include/example.c
)

# Add a static library for RoofNut
add_library(RoofNut STATIC ${SOURCE_FILES})

# Use FetchContent to retrieve GLFW and Nuklear
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

# Nuklear setup
FetchContent_Declare(
    nuklear
    GIT_REPOSITORY https://github.com/vurtun/nuklear.git
    GIT_TAG master
)
FetchContent_MakeAvailable(nuklear)

# Find Vulkan and OpenGL packages
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

# Link libraries to RoofNut
target_link_libraries(RoofNut PRIVATE
    glfw
    Vulkan::Vulkan
    OpenGL::GL
)

# Include directories for RoofNut and Nuklear headers
target_include_directories(RoofNut PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${nuklear_SOURCE_DIR}
)

# Set output directories
set_target_properties(RoofNut PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install the library and headers
install(TARGETS RoofNut DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${nuklear_SOURCE_DIR}/nuklear.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
