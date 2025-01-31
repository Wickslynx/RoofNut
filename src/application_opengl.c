#include "external/glew/include/GL/glew.h"
#include "external/glfw/include/GLFW/glfw3.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "application_opengl.h"
#include "image.h"

GLFWwindow *g_Window = NULL;
extern ImageRenderer* imageRenderer;

struct Application {
    struct RoofNutSpecification specification;
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
        struct nk_glfw glfw;
    } nuklear;
};

void DestroyOpenGL(struct Application* app) {
    if (app->nuklear.ctx) {
        nk_glfw3_shutdown(&app->nuklear.glfw);
    }
    if (imageRenderer) {
        destroyImageRenderer(imageRenderer);
    }
    if (app->windowHandle) {
        glfwDestroyWindow(app->windowHandle);
    }
    glfwTerminate();
}

bool init_nuklear(struct Application* app) {
    app->nuklear.ctx = nk_glfw3_init(&app->nuklear.glfw, app->windowHandle, NK_GLFW3_INSTALL_CALLBACKS);
    if (!app->nuklear.ctx) {
        printf("Failed to initialize Nuklear\n");
        return false;
    }

    nk_glfw3_font_stash_begin(&app->nuklear.glfw, &app->nuklear.atlas);
    nk_glfw3_font_stash_end(&app->nuklear.glfw);
    return true;
}

bool init_opengl(struct Application* app) {
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    g_Window = glfwCreateWindow(app->specification.width, 
                               app->specification.height, 
                               app->specification.name, 
                               NULL, NULL);
    if (!g_Window) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("Failed to create GLFW window: %s\n", error_description);
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
        fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(err));
        return false;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int width, height;
    glfwGetFramebufferSize(g_Window, &width, &height);
    glViewport(0, 0, width, height);

    return true;
}

void limit_frame_rate(struct Application* app) {
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

void RoofNutLoop(struct Application* app) {
    if (!init_nuklear(app)) {
        return;
    }

    app->timing.lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(g_Window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        nk_glfw3_new_frame(&app->nuklear.glfw);

        RoofNutRender();

        nk_glfw3_render(&app->nuklear.glfw, NK_ANTI_ALIASING_ON, 
                        MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        if (imageRenderer) {
            renderImage(imageRenderer);
        }

        glfwSwapBuffers(g_Window);
        limit_frame_rate(app);
    }
}

struct Application* RoofNutCreate(const struct ApplicationSpecification* specification) {
    struct Application* app = (struct Application*)malloc(sizeof(struct Application));
    if (!app) {
        printf("Allocation of Application failed\n");
        return NULL;
    }

    memcpy(&app->specification, specification, sizeof(struct ApplicationSpecification));
    
   
    app->timing.targetFrameRate = 60.0;  // Default to 60 FPS.
    app->timing.vsyncEnabled = true;     // Default vsync on.
    app->running = true;
    
    if (!init_opengl(app)) {
        free(app);
        return NULL;
    }

    app->windowHandle = g_Window;
    RoofNut_Loop(app);

    return app;
}

void RoofNutGetWindowSize(int* width, int* height) {
    if (g_Window && width && height) {
        glfwGetWindowSize(g_Window, width, height);
    }
}

void RoofNutWindowShouldClose() {
    if (g_Window) {
        glfwSetWindowShouldClose(g_Window, GLFW_TRUE);
    }
}

void RoofNutDestroy(struct Application* app) {
    if (!app) return;
    DestroyOpenGL(app);
    free(app);
}
