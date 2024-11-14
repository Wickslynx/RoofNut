#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <math.h>


// Error checking for Vulkan calls
void check_vk_result(VkResult err);

// Application configuration structure
typedef struct {
    const char* name;   // Application name
    uint32_t width;     // Application window width
    uint32_t height;    // Application window height
} ApplicationSpecification;

// Layer callback function type
typedef void (*LayerCallback)(void);

// Application structure containing application state and resources
typedef struct {
    ApplicationSpecification specification;
    GLFWwindow* windowHandle;
    bool running;

    float timeStep;
    float frameTime;
    float lastFrameTime;

    bool customTitleBar;

    size_t layerCount;
    struct Layer** layers;

    LayerCallback menubarCallback;  // Function pointer for menubar callback
} Application;

extern struct nk_context *ctx;

// Function declarations
Application* Application_Create(const ApplicationSpecification* specification);
void Application_Destroy(Application* app);
void DrawCustomTitleBar(Application* app);
void OnUiRender();
void Application_Run(Application* app);
void Application_SetMenubarCallback(Application* app, LayerCallback menubarCallback);
void Application_PushLayer(Application* app, struct Layer* layer);
void Application_Close(Application* app);

float Application_GetTime(const Application* app);
GLFWwindow* Application_GetWindowHandle(const Application* app);

VkInstance Application_GetInstance();
VkPhysicalDevice Application_GetPhysicalDevice();
VkDevice Application_GetDevice();

VkCommandBuffer Application_GetCommandBuffer(bool begin);
void Application_FlushCommandBuffer(VkCommandBuffer commandBuffer);

void Application_SubmitResourceFree(void (*func)(void));

// Implemented by the client application
Application* CreateApplication(int argc, char** argv);

