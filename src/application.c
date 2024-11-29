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
VkSurfaceKHR g_Surface;
VkSurfaceCapabilitiesKHR g_SurfaceCapabilities;
VkExtent2D g_SwapChainExtent;
VkSemaphore renderSemaphore;
GLFWwindow* g_Window;
VkQueue g_Queue;
VkCommandBuffer g_CommandBuffer;
VkCommandPool g_CommandPool;
uint32_t queueFamilyIndex = 0;
VkSwapchainKHR g_Swapchain; // Add this to hold the swapchain.
int imageIndex;
VkImageView* imageViews = NULL;  // To hold image views
VkImage* swapchainImages = NULL; // To hold swapchain images
VkSurfaceFormatKHR selectedFormat; // To hold selected surface format


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
        .pApplicationName = "RoofNut application.", // Set the application name.
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0), // Set the application version.
        .pEngineName = "RoofNut", // Set the engine name.
        .engineVersion = VK_MAKE_VERSION(1, 5, 1), // Set the engine version.
        .apiVersion = VK_API_VERSION_1_0 // Set the API version (1).
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo
    };

    VkResult res = vkCreateInstance(&createInfo, NULL, &g_Instance);
    check_vk_result(res);  // Check instance creation.

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(g_Device, g_Swapchain, &imageCount, NULL);
}

// Initialization of device.
void init_device() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        printf("Failed to find GPUs with Vulkan support.\n");
        exit(1);
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
    check_vk_result(res);  // Check device 
}

void init_swapchain() {

    imageViews = malloc(sizeof(VkImageView) * swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
    	VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = swapchainImages[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = selectedFormat.format,
        .subresourceRange = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    vkCreateImageView(g_Device, &viewInfo, NULL, &imageViews[i]);
    // Ensure GLFW has been initialized and the window is created before calling this function
    glfwCreateWindowSurface(g_Instance, g_Window, NULL, &g_Surface); // Create a window surface.

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_PhysicalDevice, g_Surface, &formatCount, NULL); // Check for available surface formats.
    if (formatCount == 0) {
        printf("No surface formats found.");
        exit(EXIT_FAILURE);
    }

    VkSurfaceFormatKHR surfaceFormats[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_PhysicalDevice, g_Surface, &formatCount, surfaceFormats);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_PhysicalDevice, g_Surface, &g_SurfaceCapabilities); // Get the surface format capabilities.

    // Select surface format
    VkSurfaceFormatKHR selectedFormat;
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        selectedFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        selectedFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    } else {
        selectedFormat = surfaceFormats[0];
    }

    // Select present mode
    VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR; // Select the default mode.

    uint32_t imageCount = g_SurfaceCapabilities.minImageCount + 1;
    if (g_SurfaceCapabilities.maxImageCount > 0 && imageCount > g_SurfaceCapabilities.maxImageCount) {
        imageCount = g_SurfaceCapabilities.maxImageCount;
    }

    VkExtent2D swapchainExtent = g_SurfaceCapabilities.currentExtent;
    swapchainExtent.width = 800; // Example width; replace with actual app width if needed.
    swapchainExtent.height = 600; // Example height; replace with actual app height if needed.

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = g_Surface,
        .minImageCount = imageCount,
        .imageFormat = selectedFormat.format,
        .imageColorSpace = selectedFormat.colorSpace,
        .imageExtent = swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = g_SurfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = selectedPresentMode,
        .clipped = VK_FALSE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    VkResult res = vkCreateSwapchainKHR(g_Device, &swapchainCreateInfo, NULL, &g_Swapchain);
    if (res != VK_SUCCESS) {
        printf("Failed to create swapchain\n");
        exit(EXIT_FAILURE);
    }
}

void init_buffers() {
    // Allocate space for framebuffers
    g_Framebuffer = malloc(sizeof(VkFramebuffer) * imageCount);


    for (uint32_t i = 0; i < imageCount; ++i) {
    	VkFramebufferCreateInfo framebufferInfo = {
        	.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        	.renderPass = g_RenderPass,
        	.attachmentCount = 1,
        	.pAttachments = &imageViews[i],
        	.width = g_SwapChainExtent.width,
        	.height = g_SwapChainExtent.height,
        	.layers = 1
	}
    };

    vkCreateFramebuffer(g_Device, &framebufferInfo, NULL, &g_Framebuffer[i]);
}


    // Create command pool
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndex,
    };
    check_vk_result(vkCreateCommandPool(g_Device, &poolInfo, NULL, &g_CommandPool));

    // Allocate command buffer
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = g_CommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    check_vk_result(vkAllocateCommandBuffers(g_Device, &allocInfo, &g_CommandBuffer));
}



void render_pass() {
    //Define the color attachments (image).
    VkAttachmentDescription colorAttachment = {
        .format = VK_FORMAT_B8G8R8A8_UNORM,  // Format of the swapchain image
        .samples = VK_SAMPLE_COUNT_1_BIT,   // No multisampling.
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // Clear the color attachment at the start.
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // Store the result at the end.
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,  // No initial layout.
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  // Layout after rendering (ready for presentation).
    }; 

    //Reference the attachment.
    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,  // Index of the attachment
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // Use optimal layout for color attachment
    };

    //Define the subpass to use the color attachments.
    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,  // Graphics pipeline
        .colorAttachmentCount = 1,  // Number of color attachments
        .pColorAttachments = &colorAttachmentRef  // Reference to the color attachment
    };
    // Define the render pass info.
    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,  // One attachment (swapchain image)
        .pAttachments = &colorAttachment,  // Attachments
        .subpassCount = 1,  // One subpass
        .pSubpasses = &subpass  // Subpass definition
    };

    // Create the render pass.
    VkResult res = vkCreateRenderPass(g_Device, &renderPassInfo, NULL, &g_RenderPass);
    check_vk_result(res);
    
    printf("Render pass done.");
} 

// Main application function, used to create the application.
Application* Application_Create(const ApplicationSpecification* specification) {
    // Initialize GLFW
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return NULL;
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

    g_Window = app->windowHandle; // Assign to global variable

    return app; // Return the app if everything is fine.
}
// Main rendering loop
void main_loop() {

    init_device();
    init_vulkan();
    init_swapchain();
    init_buffers();

    
    while (!glfwWindowShouldClose(g_Window)) {
        glfwPollEvents(); // Process window events

        // Start a new frame
        vkQueueWaitIdle(g_Queue); // Ensure the previous frame is finished

        // Record command buffer
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        vkBeginCommandBuffer(g_CommandBuffer, &beginInfo);

        VkClearValue clearValue = { .color = {{0.0f, 0.0f, 0.0f, 1.0f}} };
        VkRenderPassBeginInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = g_RenderPass,
            .framebuffer = g_Framebuffer,
            .renderArea = {
                .offset = {0, 0},
                .extent = g_SwapChainExtent
            },
            .clearValueCount = 1,
            .pClearValues = &clearValue
        };

        vkCmdBeginRenderPass(g_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Add drawing commands here (e.g., vkCmdDraw)

        vkCmdEndRenderPass(g_CommandBuffer);
        vkEndCommandBuffer(g_CommandBuffer);

        // Submit command buffer
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &g_CommandBuffer
        };

        vkQueueSubmit(g_Queue, 1, &submitInfo, VK_NULL_HANDLE);

        // Present frame
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .swapchainCount = 1,
            .pSwapchains = &g_Swapchain,
            .pImageIndices = &imageIndex,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderSemaphore
        };

        vkQueuePresentKHR(g_Queue, &presentInfo);
    }

    vkDeviceWaitIdle(g_Device); // Ensure all GPU tasks are finished before exiting
}


// Main application shutdown and cleanup function.
void Application_Destroy(Application* app) {
    if (!app) return;

    // Clean up Vulkan resources
    vkDestroySwapchainKHR(g_Device, g_Swapchain, NULL);
    vkDestroySurfaceKHR(g_Instance, g_Surface, NULL);
    vkDestroyDevice(g_Device, NULL);
    vkDestroyInstance(g_Instance, NULL);

    // Clean up GLFW
    glfwDestroyWindow(app->windowHandle);  // Destroy the GLFW window.
    glfwTerminate();  // Terminate GLFW.
    free(app); // Free the app.
}
