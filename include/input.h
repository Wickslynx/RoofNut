#pragma once
#include "external/glfw/include/GLFW/glfw3.h"

//Declare functions.
void Input_Init(GLFWwindow* window);
void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
int isKeyPressed(int button);

int isMouseButtonPressed(int button);

void getMousePosition(double* x, double* y);
