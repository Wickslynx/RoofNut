# Make sure your CMake version is compatible
cmake_minimum_required(VERSION 3.14)

# Define the project name
project(RoofNut)

# Add the header and source files for the library
set(SOURCE_FILES src/application.c src/input.c src/Utilites.c)

# Create a static library (use SHARED for dynamic/shared library)
add_library(RoofNut STATIC ${SOURCE_FILES})

# Link any necessary libraries here
target_link_libraries(RoofNut glfw Vulkan::Vulkan OpenGL::GL)

# Include directories for your header files
target_include_directories(RoofNut PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Fetch content for GLFW (or any other dependencies)
include(FetchContent)
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

# Optionally, if you need any additional libraries or link directories
# target_link_libraries(RoofNut ${SOME_OTHER_LIBS})
