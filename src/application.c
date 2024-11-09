#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "librarys/Nuklear/nuklear.h"
#include "librarys/Nuklear/nuklear_glfw_vulkan.h"

// Global Vulkan resources
VkInstance g_Instance;
VkPhysicalDevice g_PhysicalDevice;
VkDevice g_Device;
GLFWwindow* g_Window;
VkQueue g_Queue;
uint32_t queueFamilyIndex = 0; // The queue family index where graphics operations are supported

// Global Nuklear context
struct nk_context *ctx;

// Function to check Vulkan results
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan Error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

// Initialize Vulkan, GLFW, and Nuklear
int nk_glfw_vulkan_init(GLFWwindow* window, struct nk_context** ctx) {
    *ctx = (struct nk_context*)malloc(sizeof(struct nk_context));
    if (*ctx == NULL) {
        return -1;  // Allocation failed
    }
    return 0;
}

// Render Nuklear UI with Vulkan
void nk_glfw_vulkan_render(struct nk_context* ctx) {
    // This is where you would render Nuklear UI with Vulkan commands
    // Vulkan rendering code goes here, including command buffers, pipelines, etc.
}

// Shutdown Nuklear and Vulkan
void nk_glfw_vulkan_shutdown(struct nk_context* ctx) {
    free(ctx);  // Free Nuklear context
}

// Create and initialize the Application
Application* Application_Create(const ApplicationSpecification* specification) {
    Application* app = (Application*)malloc(sizeof(Application)); // Malloc the size of the application
    if (!app) {
        printf("Memory allocation failed.\n");
        return NULL; // Memory allocation failed
    }

    app->specification = *specification;
    app->running = false;
    app->customTitleBar = false;
    app->timeStep = 0.0f;
    app->frameTime = 0.0f;
    app->lastFrameTime = 0.0f;
    app->layerCount = 0;
    app->layers = NULL;
    app->menubarCallback = NULL;

    // Initialize GLFW
    if (!glfwInit()) {
        free(app);
        return NULL; // GLFW initialization failed
    }

    // If custom title bar is enabled, handle it
    if (app->customTitleBar != false) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        DrawCustomTitleBar(app);
    }

    // Create the window
    app->windowHandle = glfwCreateWindow(app->specification.width, app->specification.height, app->specification.name, NULL, NULL);
    if (!app->windowHandle) {
        glfwTerminate();
        free(app);
        return NULL; // Window creation failed
    }

    // Initialize Nuklear with Vulkan
    if (nk_glfw_vulkan_init(app->windowHandle, &ctx) != 0) {
        printf("Failed to initialize Nuklear.\n");
        free(app);
        return NULL; // Nuklear initialization failed
    }

    return app;
}

// Clean up and destroy the Application
void Application_Destroy(Application* app) {
    if (!app) return;

    // Shutdown Nuklear and Vulkan resources
    nk_glfw_vulkan_shutdown(ctx);

    glfwDestroyWindow(app->windowHandle);
    glfwTerminate();
    free(app);
}

// Custom title bar drawing function (Placeholder)
void DrawCustomTitleBar(Application* app) {
    // NEED TO BE IMPLEMENTED
}

// Run the application main loop
void Application_Run(Application* app) {
    app->running = true;

    while (app->running && !glfwWindowShouldClose(app->windowHandle)) {
        glfwPollEvents();

        // Create a new frame for Nuklear
        nk_glfw_vulkan_new_frame(ctx);
        
        //Users code will be run here.
        OnUiRender();

        // Render Nuklear UI with Vulkan
        nk_glfw_vulkan_render(ctx);

        // Swap buffers
        glfwSwapBuffers(app->windowHandle);

        // Update timing
        float time = Application_GetTime(app);
        app->frameTime = time - app->lastFrameTime;
        app->timeStep = fminf(app->frameTime, 0.0333f); // Cap time step at ~30fps
        app->lastFrameTime = time;
    }
}

// Get the current time
float Application_GetTime(const Application* app) {
    return (float)glfwGetTime();
}

// Get the GLFW window handle
GLFWwindow* Application_GetWindowHandle(const Application* app) {
    return app->windowHandle;
}

// Vulkan instance retrieval
VkInstance Application_GetInstance() {
    return g_Instance;
}

// Vulkan physical device retrieval
VkPhysicalDevice Application_GetPhysicalDevice() {
    return g_PhysicalDevice;
}

// Vulkan device retrieval
VkDevice Application_GetDevice() {
    return g_Device;
}

// Command buffer allocation
VkCommandBuffer Application_GetCommandBuffer(bool begin) {
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = VK_NULL_HANDLE, // Placeholder for actual command pool
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    
    check_vk_result(vkAllocateCommandBuffers(g_Device, &allocateInfo, &commandBuffer));

    if (begin) {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };
        check_vk_result(vkBeginCommandBuffer(commandBuffer, &beginInfo));
    }

    return commandBuffer;
}

// Flush the command buffer
void Application_FlushCommandBuffer(VkCommandBuffer commandBuffer) {
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer
    };

    check_vk_result(vkEndCommandBuffer(commandBuffer));
    check_vk_result(vkQueueSubmit(g_Queue, 1, &submitInfo, VK_NULL_HANDLE));
    check_vk_result(vkQueueWaitIdle(g_Queue));  // Wait for GPU to finish
}

