# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(RoofNut VERSION 1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)



add_subdirectory(include/external/glfw)


find_package(Vulkan QUIET)

if (Vulkan_FOUND)
    message(STATUS "Using Vulkan for rendering.")
    add_definitions(-DUSE_VULKAN)
    set(SOURCES
        src/application_vulkan.c
        src/input.c
        src/Utilites.c
    )
    set(HEADERS
        include/application_vulkan.h
        include/input.h
        include/Utilites.h
        include/roofnut.h
    )
    set(GRAPHICS_LIBRARIES Vulkan::Vulkan)

else()
    message(STATUS "Using built-in OpenGL for rendering.")

    add_definitions(-DUSE_OPENGL)
    
    add_subdirectory(include/external/glew/build/cmake)

    set(SOURCES
        src/application_opengl.c
        src/input.c
        src/Utilites.c
    )
    set(HEADERS
        include/application_opengl.h
        include/input.h
        include/Utilites.h
        include/roofnut.h
    )

    find_package(GLEW REQUIRED) 
    set(EXTRA_LIBRARIES GLEW::GLEW)
    set(GRAPHICS_LIBRARIES OpenGL::GL)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/glfw/include/GLFW
    ${CMAKE_SOURCE_DIR}/include/librarys/Nuklear
)

# Link the appropriate libraries
target_link_libraries(RoofNut PRIVATE glfw ${GRAPHICS_LIBRARIES} ${EXTRA_LIBRARIES} m)

# Installation targets
install(TARGETS RoofNut
    DESTINATION /usr/local/lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION /usr/local/include
    FILES_MATCHING PATTERN "*.h"
)


# Ignore this, It's made for my testing (Wickslynx).
add_executable(RoofNutTest test.c)

# Ignore this, made for my testing (Wickslynx).
target_link_libraries(RoofNutTest PRIVATE RoofNut glfw ${EXTRA_LIBRARIES} ${GRAPHICS_LIBRARIES} m)
