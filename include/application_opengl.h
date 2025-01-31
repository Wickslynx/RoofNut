#ifndef APPLICATION_OPENGL_H
#define APPLICATION_OPENGL_H

#include <stdbool.h>


// Forward declarations:
struct nk_context;
struct nk_font_atlas;
struct nk_glfw;

#define MAX_VERTEX_MEMORY 512 * 1024

#define MAX_ELEMENT_MEMORY 128 * 1024

struct Application;


struct ApplicationSpecification {
    const char* name;
    int width;
    int height;
};


// Function declarations:
struct Application* RoofNutCreate(const struct ApplicationSpecification* specification);
void RoofNutDestroy(struct Application* app);
void RoofNutGetWindowSize(int* width, int* height);
void RoofNutWindowShouldClose(void);
void RoofNutRender();
static void DestroyOpenGL(struct Application* app);
static void limit_frame_rate(struct Application* app);

#endif // APPLICATION_OPENGL_H
