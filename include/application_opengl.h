

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>   // Include <stdbool.h> for bool type
#include <string.h>
#include <GLFW/glfw3.h>

extern GLFWwindow *g_Window;

void init_opengl(void);
void RoofNut_loop(void);
void DestroyOpenGl(void);
void UiRender();

struct ApplicationSpecification {
    const char* name;
    int width;
    int height;
};

struct Application {
    struct ApplicationSpecification specification;
    bool running;
    bool customTitleBar;
};

struct Application* Application_Create(const struct ApplicationSpecification* specification);
void Application_Destroy(struct Application* app);

