#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global variables for Vulkan and GLFW resources
VkInstance g_Instance;
VkPhysicalDevice g_PhysicalDevice;
VkDevice g_Device;
GLFWwindow* g_Window;

// Function to check Vulkan results
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan Error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}


// Create and initialize the Application
Application* Application_Create(const ApplicationSpecification* specification) {
    Application* app = (Application*)malloc(sizeof(Application)); //Malloc the size of the application.
    if (!app) {
        printf("Memory allocation failed.")
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

    //If, Customtitlebar is enabled, Remove old titlebar and draw new titlebar !BETA!.

    if (app->customtitlebar != false) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        DrawCustomTitleBar(&app);
    };

    // Create the window
    app->windowHandle = glfwCreateWindow(app->specification.width, app->specification.height, app->specification.name, NULL, NULL);
    if (!app->windowHandle) {
        glfwTerminate();
        free(app);
        return NULL; //Window creation failed. 
    }

    return app;
}

// Clean up and destroy the Application
void Application_Destroy(Application* app) {
    if (!app) return;

    glfwDestroyWindow(app->windowHandle);
    glfwTerminate();
    free(app);
}


void DrawCustomTitleBar(Application* app) {
    //NEED TO BE IMPLEMENTED.
}

// Run the application main loop
void Application_Run(Application* app) {
    app->running = true;

    while (app->running && !glfwWindowShouldClose(app->windowHandle)) {
        glfwPollEvents()
        //If, Customtitlebar is enabled, Remove old titlebar and draw new titlebar !BETA!.
        if (app->customtitlebar == true) {
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            DrawCustomTitleBar(&app);
        };

        // Update layers and render UI
        for (size_t i = 0; i < app->layerCount; ++i) {
            if (app->layers[i] != NULL) {
                app->layers[i]->OnUpdate(app->timeStep); // Assume Layer has an OnUpdate method
                app->layers[i]->OnUIRender(); // Assume Layer has an OnUIRender method
            }
        }


        // Swap buffers
        glfwSwapBuffers(app->windowHandle);

        // Update timing
        float time = Application_GetTime(app);
        app->frameTime = time - app->lastFrameTime;
        app->timeStep = fminf(app->frameTime, 0.0333f); // Cap time step at ~30fps
        app->lastFrameTime = time;
    }
}
void OnUIRender() {
    
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
        .commandPool = VK_NULL_HANDLE, /
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
    // Submit to the graphics queue (assuming g_Queue is defined elsewhere)
    check_vk_result(vkQueueSubmit(VK_NULL_HANDLE, 1, &submitInfo, VK_NULL_HANDLE)); // Placeholder for actual queue handle
    check_vk_result(vkQueueWaitIdle(VK_NULL_HANDLE)); // Placeholder for actual queue handle
}

// Submit a resource cleanup function
void Application_SubmitResourceFree(void (*func)(void)) {
    //NEED TO BE IMPLEMTENTED !NOTE! Number 2.
}



