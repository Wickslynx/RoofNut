#pragma once
#include <GLFW/glfw3.h>


void Input_Init(GLFWwindow* window);

int isKeyPressed(int button);

int isMouseButtonPressed(int button);

void getMousePosition(double* x, double* y);