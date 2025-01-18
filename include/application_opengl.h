
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>   // Include <stdbool.h> for bool type
#include <string.h>

#include <GL/glew.h>
#include "external/glfw/include/GLFW/glfw3.h"


//Define and set the GLFW window to null. 
extern GLFWwindow *g_Window; 
GLFWwindow* g_Window = NULL;


struct Application {
    struct ApplicationSpecification specification;
    bool running;
    bool customTitleBar;
    GLFWwindow* windowHandle;
};


void init_opengl(void);
void RoofNut_loop(void);
void DestroyOpenGl(void);
void UiRender();

struct ApplicationSpecification {
    const char* name;
    int width;
    int height;
};



struct Application* Application_Create(const struct ApplicationSpecification* specification);
void Application_Destroy(struct Application* app);

