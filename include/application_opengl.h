#ifndef APPLICATION_OPENGL
#define APPLICATION_OPENGL

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>   
#include <string.h>


//Define and set the GLFW window to null. 
extern GLFWwindow *g_Window; 
GLFWwindow* g_Window = NULL;


struct ApplicationSpecification {
    const char* name;
    int width; 
    int height; 
};



void init_opengl(void);
void RoofNut_loop(void);
void DestroyOpenGl(void);
void UiRender();




struct Application* Application_Create(const struct ApplicationSpecification* specification);
void Application_Destroy(struct Application* app);

#endif 
