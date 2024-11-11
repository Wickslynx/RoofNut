# RoofNut CMakeLists.txt

cmake_minimum_required(VERSION 3.14)
project(RoofNut)

# Set the source files for the library
set(SOURCE_FILES 
    src/application.c
    src/input.c
    src/Utilites.c
    include/example.c
)

# Add a static library
add_library(RoofNut STATIC ${SOURCE_FILES})

# Fetch and include GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)

set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland support")
set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 support")
FetchContent_MakeAvailable(glfw)

# Fetch and include Nuklear
FetchContent_Declare(
    nuklear
    GIT_REPOSITORY https://github.com/vurtun/nuklear.git
    GIT_TAG master
)
FetchContent_MakeAvailable(nuklear)

# Find Vulkan and OpenGL packages
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(nuklear REQUIRED)

# Link libraries
target_link_libraries(RoofNut PRIVATE
    glfw
    Vulkan::Vulkan
    OpenGL::GL
    nuklear
)

# Include directories for RoofNut and Nuklear headers
target_include_directories(RoofNut PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${nuklear_SOURCE_DIR}
)

# Install the library and headers
install(TARGETS RoofNut DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${nuklear_SOURCE_DIR}/nuklear.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
