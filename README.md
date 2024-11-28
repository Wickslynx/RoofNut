# RoofNut

**Version 1.5**

RoofNut is a lightweight GUI framework inspired by Walnut, using GLFW and Vulkan for rendering, and Nuklear for the GUI. It's an evolving library designed for high performance with a customizable, immediate-mode interface.

# Features:
Built on GLFW and Vulkan for efficient rendering.
Uses Nuklear for immediate-mode GUI.
# Getting Started:

**1. Install dependencies:**
Run this command to install required libraries automatically: 

***-NOTE-***
The only command that has been proven to work from testing is the *Debian* command. **The commands for the other distros might not work as expected.**

For Debian-based systems: * sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libglfw3-dev libvulkan-dev libxi-dev *

For Fedora-based systems (DNF): * sudo dnf install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel glfw-devel vulkan-loader-devel libXi-devel * 

For CentOS/RHEL systems (YUM): * sudo yum install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel glfw-devel vulkan-loader-devel libXi-devel * 

For Arch-based systems: * sudo pacman -S libx11 libxrandr libxinerama libxcursor glfw-x11 vulkan-loader libxi *

For OpenSUSE (Zypper Package Manager): sudo zypper install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel glfw3-devel vulkan-devel libXi-devel



**2. Compile and build your application using RoofNut.**
Implement the main rendering function using OnUiRender().


Please see the *example.c* file for further explanation and examples


# Current Issues:

**1. Linker not linking**, Unknown function declarations when included library.
**2. Segmentation fault**, When running program with compatible drives i get "Segmentationfault".
**3. What is happening????**, We devs don't know what this has become? Where should it go? What should we do?

***RoofNut is still in development, no stable version has released.***
