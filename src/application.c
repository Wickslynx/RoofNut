#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global variables for Vulkan and GLFW resources
VkInstance g_Instance;
VkPhysicalDevice g_PhysicalDevice;
VkDevice g_Device;
GLFWwindow* g_Window;
VkQueue g_Queue;
uint32_t queueFamilyIndex = 0; // The queue family index where graphics operations are supported
vkGetDeviceQueue(g_Device, queueFamilyIndex, 0, &g_Queue);

// Global Nuklear context
struct nk_context *ctx;

void OnGUiRender() {
    
}
// Function to check Vulkan results
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan Error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

// Function to initialize Nuklear with Vulkan and GLFW
int nk_glfw_vulkan_init(GLFWwindow* window, struct nk_context** ctx) {
    // Initialize Nuklear with Vulkan and GLFW here
    // This is a placeholder function, you will need to find or implement a proper Vulkan backend for Nuklear
    *ctx = (struct nk_context*)malloc(sizeof(struct nk_context));
    if (*ctx == NULL) {
        return -1;  // Allocation failed
    }
    // Initialization code for Vulkan, GLFW, and Nuklear backend should go here
    // You should set up Vulkan command buffers, pipelines, and shaders for rendering the UI
    return 0;
}

// Function to render Nuklear UI with Vulkan
void nk_glfw_vulkan_render(struct nk_context* ctx) {
    // This is a placeholder for rendering Nuklear UI with Vulkan commands
    // Vulkan rendering code goes here, including command buffers, pipelines, and swapchain management
}

// Shutdown Nuklear and Vulkan
void nk_glfw_vulkan_shutdown(struct nk_context* ctx) {
    // Cleanup Nuklear resources
    free(ctx);  // Free Nuklear context
    // Cleanup Vulkan resources if necessary
}

// Create and initialize the Application
Application* Application_Create(const ApplicationSpecification* specification) {
    Application* app = (Application*)malloc(sizeof(Application)); // Malloc the size of the application.
    if (!app) {
        printf("Memory allocation failed.\n");
        return NULL; // Memory allocation failed
    }

    app->specification = *specification;
    app->running = false;
    app->customtitlebar = false;
    app->timeStep = 0.0f;
    app->frameTime = 0.0f;
    app->lastFrameTime = 0.0f;
    app->layerCount = 0;
    app->layers = NULL; // Initialize layers to NULL
    app->menubarCallback = NULL;

    // Initialize GLFW
    if (!glfwInit()) {
        free(app);
        return NULL; // GLFW initialization failed
    }

    // If, Customtitlebar is enabled, Remove old titlebar and draw new titlebar !BETA!.
    if (app->customtitlebar != false) {
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

    // Initialize Nuklear
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

// Custom title bar drawing function (Not implemented)
void DrawCustomTitleBar(Application* app) {
    // NEED TO BE IMPLEMENTED.
}

// Run the application main loop
void Application_Run(Application* app) {
    app->running = true;

    while (app->running && !glfwWindowShouldClose(app->windowHandle)) {
        glfwPollEvents();

        /
        nk_glfw_vulkan_new_frame(ctx);

        // Create your Nuklear window and widgets here
        OnGuiRender();

        // Render the Nuklear UI with Vulkan
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

// Set the menubar callback function
void Application_SetMenubarCallback(Application* app, LayerCallback menubarCallback) {
    app->menubarCallback = menubarCallback;
}

// Push a layer to the application
void Application_PushLayer(Application* app, struct Layer* layer) {
    app->layers = (struct Layer**)realloc(app->layers, sizeof(struct Layer*) * (app->layerCount + 1));
    if (app->layers) {
        app->layers[app->layerCount] = layer; // Store the new layer
        app->layerCount++;
        layer->OnAttach(); // Call OnAttach for the new layer
    }
}

// Close the application
void Application_Close(Application* app) {
    app->running = false;
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

    // Assuming a command pool is available, allocate command buffer here
    // VkCommandPool commandPool = ...; // Placeholder for command pool
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

   // Assuming g_Device is your Vulkan device and g_Queue is your Vulkan queue handle

// Allocate the command buffer and record commands (this part is handled earlier in your code)

// End the command buffer recording
check_vk_result(vkEndCommandBuffer(commandBuffer));

// Submit the command buffer to the Vulkan queue
VkSubmitInfo submitInfo = {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .commandBufferCount = 1,
    .pCommandBuffers = &commandBuffer,
    .signalSemaphoreCount = 0, // Optional semaphore for synchronization
    .pSignalSemaphores = NULL
};


    check_vk_result(vkQueueSubmit(g_Queue, 1, &submitInfo, VK_NULL_HANDLE));
    check_vk_result(vkQueueWaitIdle(g_Queue));  // Wait for the GPU to finish

}

// Submit a resource cleanup function
void Application_SubmitResourceFree(void (*func)(void)) {
    // NEED TO BE IMPLEMENTED !NOTE! Number 2.
}
