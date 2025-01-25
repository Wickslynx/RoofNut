
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>   
#include <string.h>


#define MAX_VERTEX_MEMORY 512 * 1024

#define MAX_ELEMENT_MEMORY 128 * 1024

struct Application;

struct ApplicationSpecification {
    const char* name;
    int width; 
    int height; 
};



void init_opengl(struct Application* app);
void RoofNut_loop(struct Application* app);
void DestroyOpenGl(void);
void RoofNutRender();




struct Application* Application_Create(const struct ApplicationSpecification* specification);
void Application_Destroy(struct Application* app);
