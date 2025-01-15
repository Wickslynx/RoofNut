#include "input.h"
#include "external/glfw/include/GLFW/glfw3.h"

static int keys[1024];
static int mouseButtons[32];
static double mouseX, mouseY;

void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = 1; //Key pressed
        } else if (action == GLFW_RELEASE) {
            keys[key] = 0; //Key released
        }
    } 
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= 0 && button < 1024) {
        if (action == GLFW_PRESS) {
            mouseButtons[button] = 1; //Key pressed
        } else if (action == GLFW_RELEASE) {
            mouseButtons[button] = 0; //Key released
        }
    } 
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = xpos;
    mouseY = ypos;
}

void Input_Init(GLFWwindow* window) {
    glfwSetKeyCallback(window, keycallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
}

//Check if a key is pressed, could be used as ex if (isKeyPressed(ESC)) {quit()}
int isKeyPressed(int key) {
    return keys[key];
}

//Check if mouse button is pressed.

int isMouseButtonPressed(int button) {
    return mouseButtons[button];
}

void getMousePosition(double* x, double* y) {
    *x = mouseX;
    *y = mouseY;
}



