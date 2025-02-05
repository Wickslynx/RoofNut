/**
@file application_opengl.c
@brief Implementation of OpenGL application.
*/

/**@brief Include all nessecary headers.*/
#include "external/glew/include/GL/glew.h"
#include "external/glfw/include/GLFW/glfw3.h"


#include "application_opengl.h"
#include "image.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**@brief Set up the Nuklear Context.*/
struct nk_context *ctx;

/**@brief Set up the main Application struct.*/
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
        struct nk_font_atlas *atlas;
        struct nk_glfw *glfw;  
    } nuklear;
};

/**@brief Define the nuklear implementation:*/
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "external/Nuklear/nuklear.h"
#include "external/Nuklear/nuklear_glfw_gl3.h"

/**@brief Global variables:*/
static GLFWwindow *g_Window; 
extern ImageRenderer* imageRenderer;  

/**
*@brief Destroys the OpenGL and nuklear contextes.
*@param app The main application struct.
*/
static void DestroyOpenGL(struct Application* app) {
    if (ctx) {
        nk_glfw3_shutdown(app->nuklear.glfw);
    }
    if (imageRenderer) {
        destroyImageRenderer(imageRenderer);
    }
    if (app->windowHandle) {
        glfwDestroyWindow(app->windowHandle);
    }
    glfwTerminate();
}

/**
*@brief Initializes Nuklear context.
*@param app The main application struct.
*/

static bool init_nuklear(struct Application* app) {
    app->nuklear.glfw = malloc(sizeof(struct nk_glfw));
    if (!app->nuklear.glfw) {
        return false;
    }
    
    ctx = nk_glfw3_init(app->nuklear.glfw, app->windowHandle, NK_GLFW3_INSTALL_CALLBACKS);
    if (!ctx) {
        free(app->nuklear.glfw);
        return false;
    }

    nk_glfw3_font_stash_begin(app->nuklear.glfw, &app->nuklear.atlas);
    nk_glfw3_font_stash_end(app->nuklear.glfw);
    return true;
}

/**
*@brief Initializes OpenGL context.
*@param app The main application struct.
*/

static bool init_opengl(struct Application* app) {
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
    
    #ifdef _WIN32
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
        printf("Detected windows: Running RoofNut WIN-experimental.");
    #elif defined(__APPLE__)
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        printf("Detected Apple: Running RoofNut MAC-experimental.");
    #endif

    g_Window = glfwCreateWindow(app->specification.width, 
                               app->specification.height, 
                               app->specification.name, 
                               NULL, NULL);
    if (!g_Window) {
        return false;
    }

    glfwMakeContextCurrent(g_Window);
    
    if (app->timing.vsyncEnabled) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        return false;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    app->windowHandle = g_Window;
    return true;
}

/**
*@brief Limits the frame rate. 
*@param app The main application struct.
*/

static void limit_frame_rate(struct Application* app) {
    if (app->timing.targetFrameRate <= 0) return;

    double currentTime = glfwGetTime();
    double frameTime = currentTime - app->timing.lastFrameTime;
    double targetFrameTime = 1.0 / app->timing.targetFrameRate;

    if (frameTime < targetFrameTime) {
        double sleepTime = targetFrameTime - frameTime;
        glfwWaitEventsTimeout(sleepTime);
    }

    app->timing.lastFrameTime = glfwGetTime();
}

static void RoofNut_Loop(struct Application* app) {
    if (!init_nuklear(app)) {
        return;
    }

    app->timing.lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(g_Window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        
        nk_glfw3_new_frame(app->nuklear.glfw);
        RoofNutRender();  
        nk_glfw3_render(app->nuklear.glfw, NK_ANTI_ALIASING_ON, 
                        MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        if (imageRenderer) {
            renderImage(imageRenderer);
        }

        glfwSwapBuffers(g_Window);
        limit_frame_rate(app);
    }
}

struct Application* RoofNutCreate(const struct ApplicationSpecification* specification) {
    struct Application* app = calloc(1, sizeof(struct Application));
    if (!app) {
        return NULL;
    }

    memcpy(&app->specification, specification, sizeof(struct ApplicationSpecification));
    
    app->timing.targetFrameRate = 60.0;
    app->timing.vsyncEnabled = true;
    app->running = true;
    
    if (!init_opengl(app)) {
        RoofNutDestroy(app);
        return NULL;
    }

    RoofNut_Loop(app);
    return app;
}

void RoofNutGetWindowSize(int* width, int* height) {
    if (g_Window && width && height) {
        glfwGetWindowSize(g_Window, width, height);
    }
}

void RoofNutWindowShouldClose(void) {
    if (g_Window) {
        glfwSetWindowShouldClose(g_Window, GLFW_TRUE);
    }
}

void RoofNutDestroy(struct Application* app) {
    if (!app) return;
    DestroyOpenGL(app);
    free(app);
}
