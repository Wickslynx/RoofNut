cmake_minimum_required(VERSION 3.10)
project(RoofNut)

# Find the Vulkan package if needed
find_package(Vulkan REQUIRED)

# Add the executable and link Vulkan libraries
add_executable(RoofNut include/roofnut.c, include/application.c)  # Replace main.c with the actual file if different

# Link libraries, such as Vulkan, if needed
target_link_libraries(RoofNut Vulkan::Vulkan)
