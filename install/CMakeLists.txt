# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(RoofNut VERSION 1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True) 

set(OpenGL_GL_PREFERENCE GLVND)

# Define the parent directory (one level up from the cmake directory)
set(PARENT_DIR ${CMAKE_SOURCE_DIR}/..)

# Add GLFW subdirectory
add_subdirectory(${PARENT_DIR}/include/external/glfw ${CMAKE_BINARY_DIR}/glfw)


find_package(Vulkan QUIET)
find_package(OpenGL QUIET)

if (OenGL_FOUND)
    # Add GLEW subdirectory
    add_subdirectory(${PARENT_DIR}/include/external/glew/build/cmake ${CMAKE_BINARY_DIR}/glew)
    set(GLEW_DIR ${PARENT_DIR}/include/external/glew/src)
    
    message(STATUS "Using OpenGL for rendering.")
    add_definitions(-DUSE_OPENGL)
    
    set(SOURCES
        ${PARENT_DIR}/src/application_opengl.c
        ${PARENT_DIR}/src/input.c
        ${PARENT_DIR}/src/image.c
    )
    set(HEADERS
        ${PARENT_DIR}/include/application_opengl.h
        ${PARENT_DIR}/include/input.h
        ${PARENT_DIR}/include/roofnut.h
        ${PARENT_DIR}/include/external/Nuklear/nuklear.h
        ${PARENT_DIR}/include/image.h
        ${PARENT_DIR}/include/external/stb/stb_image.h
    )

    # Set GLEW as an external library from subdirectory
    set(EXTRA_LIBRARIES GLEW)
    set(GRAPHICS_LIBRARIES OpenGL::GL)
    
elseif (Vulkan_FOUND)
   message(STATUS "Using Vulkan for rendering.")
    add_definitions(-DUSE_VULKAN)
    set(SOURCES
        ${PARENT_DIR}/src/application_vulkan.c
        ${PARENT_DIR}/src/input.c
        ${PARENT_DIR}/src/image.c
    )
    set(HEADERS
        ${PARENT_DIR}/include/application_vulkan.h
        ${PARENT_DIR}/include/input.h
        ${PARENT_DIR}/include/image.h
        ${PARENT_DIR}/include/roofnut.h
        ${PARENT_DIR}/include/external/Nuklear/nuklear.h
        ${PARENT_DIR}/include/external/stb/stb_image.h

    )
    set(GRAPHICS_LIBRARIES Vulkan::Vulkan)

else()
    message(FATAL_ERROR "No available drivers found, (OpenGL nor Vulkan).")
endif()

# Create the library target
add_library(RoofNut STATIC ${SOURCES} ${HEADERS})

# Define macros
if (Vulkan_FOUND)
    target_compile_definitions(RoofNut PRIVATE ROOFNUT_USE_VULKAN)
elseif (OpenGL_FOUND)
    target_compile_definitions(RoofNut PRIVATE ROOFNUT_USE_OPENGL)
endif()

# Include directories for project and dependencies
target_include_directories(RoofNut PUBLIC
    ${PARENT_DIR}/include
    ${PARENT_DIR}/include/external/glfw/include/GLFW/
    ${PARENT_DIR}/include/external/glew/include/GL/
    ${PARENT_DIR}/include/external/Nuklear/
    ${PARENT_DIR}/include/external/stb/
)

# Link the appropriate libraries
target_link_libraries(RoofNut PRIVATE glfw ${GRAPHICS_LIBRARIES} ${EXTRA_LIBRARIES} OpenGL::GL m)

# Installation targets
install(TARGETS RoofNut
    DESTINATION /usr/local/lib
)

install(DIRECTORY ${PARENT_DIR}/include/
    DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${PARENT_DIR}/include/external/nuklear/
    DESTINATION /usr/local/include/nuklear
    FILES_MATCHING PATTERN "*.h"
)

add_executable(roofnut ${PARENT_DIR}/src/roofnut-tool.c)

install(TARGETS roofnut
    DESTINATION /usr/local/bin
)
