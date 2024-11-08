cmake_minimum_required(VERSION 3.14)

project(YourVulkanProject)

# Use FetchContent to download GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)

FetchContent_MakeAvailable(glfw)


add_executable(RoofNut src/application.c, src/input.c, src/Utilites.c, src/image.c)

# Link GLFW (and Vulkan if needed)
target_link_libraries(RoofNut glfw, Vulkan::Vulkan)
