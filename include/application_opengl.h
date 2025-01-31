#ifndef APPLICATION_OPENGL_H
#define APPLICATION_OPENGL_H

#include <stdbool.h>
#include "external/glew/include/GL/glew.h"
#include "external/glfw/include/GLFW/glfw3.h"

// Forward declarations:
struct nk_context;
struct nk_font_atlas;
struct nk_glfw;

#define MAX_VERTEX_MEMORY 512 * 1024

#define MAX_ELEMENT_MEMORY 128 * 1024


struct ApplicationSpecification {
    int width;
    int height;
    const char* name;
};



struct Application {
    struct ApplicationSpecification specification;
    bool running;
    bool customTitleBar;
    GLFWwindow* windowHandle;
    struct {
        double lastFrameTime;
        double targetFrameRate;
        bool vsyncEnabled;
    } timing;
    struct {
        struct nk_context *ctx;
        struct nk_font_atlas *atlas;
        struct nk_glfw *glfw;  
    } nuklear;
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
