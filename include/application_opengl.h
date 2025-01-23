
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>   
#include <string.h>

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024



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
