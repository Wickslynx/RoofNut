# RoofNut

**Version 1.6**

RoofNut is a lightweight GUI framework inspired by Walnut, using GLFW and Vulkan or OPENGL for rendering, and Nuklear for the GUI. It's an evolving library designed for high performance with a customizable, immediate-mode interface.

# Features:
Built on GLFW and Vulkan for efficient rendering.
Uses Nuklear for immediate-mode GUI.
OpenGL support. (1.6)

# Getting Started:

**1. Install dependencies:**
Run this command to install required libraries automatically: 

***-NOTE-***
The only command that has been proven to work from testing is the *Debian* command. **The commands for the other distros might not work as expected.**

For Debian-based systems: * sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxkbcommon-dev libwayland-dev *
**(**libglew-dev or libvulkan-dev**)** 

For Fedora-based systems (DNF): * sudo dnf install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel glfw-devel vulkan-loader-devel libXi-devel * 

For Arch-based systems: * sudo pacman -S libx11 libxrandr libxinerama libxcursor glfw-x11 vulkan-loader libxi *




**2. Compile and build your application using RoofNut.**
Build your application using *cmake*, include *<roofnut.h>* and you are all set.

Please see the *example.c* file for further explanation and examples


# Current Issues:

**1. Segmentation fault (vulkan)**, When running program with compatible drivers i get "Segmentationfault". (Abandoned, *as of lack of driver support*)

**2. Additonal debugging needed (vulkan)** , (Wickslynx) can't test the vulkan version as my drivers are outdated. Hence why i added OPENGL support. (Worked on)

**3. Built in GLEW support (OpenGL)** , i can't get the compiler to reqonise the built in GLEW library.. Abandoned as of now...

***RoofNut is still in development, one stable version has released.***
