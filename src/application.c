#include "application.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


// Global Vulkan resources
VkInstance g_Instance;
VkPhysicalDevice g_PhysicalDevice;
VkDevice g_Device;
VkRenderPass g_RenderPass;
VkFramebuffer g_Framebuffer;
VkExtent2D g_SwapChainExtent;
GLFWwindow* g_Window;
VkQueue g_Queue;
VkCommandBuffer g_CommandBuffer;
VkCommandPool g_CommandPool;
uint32_t queueFamilyIndex = 0;

// Function to check Vulkan results.
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

// Initialization of Vulkan.
void init_vulkan() {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "RoofNut application.", // Set the application name. (Not visible)
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0), // Set the application version.
        .pEngineName = "RoofNut", // Set the engine name (RoofNut).
        .engineVersion = VK_MAKE_VERSION(1, 5, 1), // Set the engine version.
        .apiVersion = VK_API_VERSION_1_0 // Set the API version (1).
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo
    };

    VkResult res = vkCreateInstance(&createInfo, NULL, &g_Instance);
    check_vk_result(res);  // Check instance creation.
}

// Initialization of device.
void init_device() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        printf("Failed to find GPUs with Vulkan support.\n");
        exit(1); // Exit if no Vulkan-supported devices found.
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, devices);
    g_PhysicalDevice = devices[0]; // Select the first available GPU.

    // Select queue family.
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndex = i;
            break;
        }
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo
    };

    VkResult res = vkCreateDevice(g_PhysicalDevice, &deviceCreateInfo, NULL, &g_Device);
    check_vk_result(res);  // Check device creation.
}

// Main application function, used to create the application.
Application* Application_Create(const ApplicationSpecification* specification) {
    // Initialize GLFW
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return NULL; // If GLFW initialization fails.
    }

    // Enable Vulkan support for GLFW
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context required for Vulkan.

    // Create the window
    Application* app = (Application*)malloc(sizeof(Application));
    if (!app) {
        printf("Allocation of Application failed\n");
        glfwTerminate();  // Make sure to terminate GLFW if allocation fails.
        return NULL;
    }

    app->windowHandle = glfwCreateWindow(specification->width, specification->height, specification->name, NULL, NULL);
    if (!app->windowHandle) {
        printf("Failed to create GLFW window. \n");
        free(app);
        glfwTerminate(); // Terminate GLFW if window creation fails.
        return NULL;
    }

    return app; // Return the app if everything is fine.
}

// Main application shutdown and cleanup function.
void Application_Destroy(Application* app) {
    if (!app) return;

    // Clean up Nuklear and Vulkan resources.
    glfwDestroyWindow(app->windowHandle);  // Destroy the GLFW window.
    glfwTerminate();  // Terminate GLFW.
    free(app); // Free the app.
}
