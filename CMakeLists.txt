# RoofNut CMakeLists.txt

# Ensure the CMake version is compatible
cmake_minimum_required(VERSION 3.14)

# Define the project name
project(RoofNut)

# Set the source files for the library
set(SOURCE_FILES 
    src/application.c
    src/input.c
    src/Utilites.c
)

# Add a static library (use SHARED for a dynamic/shared library if needed)
add_library(RoofNut STATIC ${SOURCE_FILES})

# Fetch and include GLFW (make sure GLFW is built if not found)
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)
# Disable Wayland support (if not needed) and enable X11 support
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland support")
set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 support")
FetchContent_MakeAvailable(glfw)

# Find Vulkan and OpenGL packages
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)

# Link libraries
target_link_libraries(RoofNut PRIVATE
    glfw
    Vulkan::Vulkan
    OpenGL::GL
)

# Include directories for header files
target_include_directories(RoofNut PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/librarys/Nuklear
    
)

# Install the library and headers (optional)
install(TARGETS RoofNut DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
