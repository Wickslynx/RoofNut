
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
void RoofNutLoop(struct Application* app);
void limit_frame_rate(struct Application* app)
void DestroyOpenGl(void);
void RoofNutRender();




struct Application* RoofNutCreate(const struct ApplicationSpecification* specification);
void RoofNutDestroy(struct Application* app);
