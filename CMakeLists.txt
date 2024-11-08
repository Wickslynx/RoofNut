cmake_minimum_required(VERSION 3.10)
project(RoofNut)

# Find Vulkan package (Requires that the package is already installed.)
find_package(Vulkan REQUIRED)
#i
add_executable(RoofNut roofnut.c)

target_link_libraries(RoofNut Vulkan::Vulkan)
