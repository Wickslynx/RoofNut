#include "image.h"
#include "application_vulkan.h"
#include <stdlib.h>
#include <stdio.h>
#define GLFW_INCLUDE_VULKAN
#include "external/glfw/include/GLFW/glfw3.h"

#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>

// Nuklear setup.
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_IMPLEMENTATION
#define NK_GLFW_VULKAN_IMPLEMENTATION


#include "external/Nuklear/nuklear.h"
#include "external/Nuklear/nuklear_glfw_vulkan.h"


// Declares the global vulkan resources.
VkInstance g_Instance; //Declares the vulkan instance.
VkPhysicalDevice g_PhysicalDevice; //Declares the vulkan physical device.
VkDevice g_Device; //Declares the vulkan device.
VkRenderPass g_RenderPass; //Declares the renderpass.
VkFramebuffer* g_Framebuffers; //Declares the vulkan framebuffers.
VkSurfaceKHR g_Surface; //Declares the vulkan surfaces.
VkSurfaceCapabilitiesKHR g_SurfaceCapabilities;
VkExtent2D g_SwapChainExtent;
VkSemaphore renderSemaphore;
GLFWwindow* g_Window;
VkQueue g_Queue;
VkCommandBuffer g_CommandBuffer;
VkCommandPool g_CommandPool;
uint32_t queueFamilyIndex = 0;
VkSwapchainKHR g_Swapchain;
int imageIndex;
VkSemaphore imageAvailableSemaphore;
VkImageView* imageViews = NULL;
VkImage* swapchainImages = NULL;
VkSurfaceFormatKHR selectedFormat;
uint32_t swapchainImageCount = 0; 

const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

// Function to check Vulkan results.
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

void init_vulkan() {
    if (glfwVulkanSupported()) {
        printf("GLFW version - Fine.");
    } else {
        printf("Your GLFW version dosen't support vulkan, use OpenGL mode instead.");
        exit(EXIT_FAILURE);
    }
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "RoofNut application",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "RoofNut",
        .engineVersion = VK_MAKE_VERSION(1, 5, 1),
        .apiVersion = VK_API_VERSION_1_0
    };

    const char* instanceExtensions[] = { 
	    VK_KHR_SURFACE_EXTENSION_NAME, 
	    VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME, 
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    }; //Note: Wayland is required.

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = validationLayers
    };

    VkResult res = vkCreateInstance(&createInfo, NULL, &g_Instance);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan instance: %d\n", res);
        exit(EXIT_FAILURE);
    }

    printf("Vulkan instance created successfully.\n");
}


// Initialization of device.
void init_device() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        printf("Failed to find GPUs with Vulkan support.\n");
        exit(EXIT_FAILURE);
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, devices);
    g_PhysicalDevice = devices[0]; // Select the first device for simplicity

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

    const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = deviceExtensions
    };

    VkResult res = vkCreateDevice(g_PhysicalDevice, &deviceCreateInfo, NULL, &g_Device);
    if (res != VK_SUCCESS) {
        printf("Failed to create Vulkan device: %d\n", res);
        exit(EXIT_FAILURE);
    }

    vkGetDeviceQueue(g_Device, queueFamilyIndex, 0, &g_Queue);
}


void init_swapchain() {
    // Assuming g_Window is a GLFW window created with Wayland support
    VkResult res = glfwCreateWindowSurface(g_Instance, g_Window, NULL, &g_Surface);
    check_vk_result(res);

    uint32_t formatCount = 0;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(g_PhysicalDevice, g_Surface, &formatCount, NULL);
    check_vk_result(res);

    if (formatCount == 0) {
        printf("No surface formats found.\n");
        exit(EXIT_FAILURE);
    }

    VkSurfaceFormatKHR surfaceFormats[formatCount];
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(g_PhysicalDevice, g_Surface, &formatCount, surfaceFormats);
    check_vk_result(res);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_PhysicalDevice, g_Surface, &g_SurfaceCapabilities);

    selectedFormat = surfaceFormats[0];  // Simplified selection logic

    VkExtent2D swapchainExtent = g_SurfaceCapabilities.currentExtent;
    swapchainExtent.width = 800;
    swapchainExtent.height = 600;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = g_Surface,
        .minImageCount = g_SurfaceCapabilities.minImageCount + 1,
        .imageFormat = selectedFormat.format,
        .imageColorSpace = selectedFormat.colorSpace,
        .imageExtent = swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = g_SurfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_FALSE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    res = vkCreateSwapchainKHR(g_Device, &swapchainCreateInfo, NULL, &g_Swapchain);
    check_vk_result(res);

    vkGetSwapchainImagesKHR(g_Device, g_Swapchain, &swapchainImageCount, NULL);
}

void init_nuklear() {
    nk_init_default(ctx, NULL);

}

void init_buffers() {
    g_Framebuffers = malloc(sizeof(VkFramebuffer) * swapchainImageCount);
    if (!g_Framebuffers) {
	printf("Framebuffer creation failed");
	exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = g_RenderPass,
            .attachmentCount = 1,
            .pAttachments = &imageViews[i],
            .width = g_SwapChainExtent.width,
            .height = g_SwapChainExtent.height,
            .layers = 1
        };

        vkCreateFramebuffer(g_Device, &framebufferInfo, NULL, &g_Framebuffers[i]);
    }

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queueFamilyIndex,
    };
    check_vk_result(vkCreateCommandPool(g_Device, &poolInfo, NULL, &g_CommandPool));

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = g_CommandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    check_vk_result(vkAllocateCommandBuffers(g_Device, &allocInfo, &g_CommandBuffer));
}

void render_pass() {
    VkAttachmentDescription colorAttachment = {
        .format = VK_FORMAT_B8G8R8A8_UNORM,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass
    };

    VkResult res = vkCreateRenderPass(g_Device, &renderPassInfo, NULL, &g_RenderPass);
    check_vk_result(res);

    printf("Render pass done.");
}



void RoofNut_loop() {
    init_vulkan();
    init_device();        
    init_swapchain();
    init_buffers();
	
    init_nuklear(); // Note: Uncomment to use Nuklear.

    while (!glfwWindowShouldClose(g_Window)) {
        glfwPollEvents();

        vkQueueWaitIdle(g_Queue);
        //NK rendering code here.

        nk_glfw3_new_frame();


	    nk_input_begin(ctx); 
	    RoofNutRender();
	    nk_input_end(ctx);

	    
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        vkBeginCommandBuffer(g_CommandBuffer, &beginInfo);

        VkClearValue clearValue = { .color = {{0.0f, 0.0f, 0.0f, 1.0f}} };
        VkRenderPassBeginInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = g_RenderPass,
            .framebuffer = g_Framebuffers[imageIndex],
            .renderArea = {
                .offset = {0, 0},
                .extent = g_SwapChainExtent
            },
            .clearValueCount = 1,
            .pClearValues = &clearValue
        };

        vkCmdBeginRenderPass(g_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	    enum nk_anti_aliasing AA = NK_ANTI_ALIASING_ON;

	    nk_glfw3_render(g_Queue, imageIndex, imageAvailableSemaphore, AA); //  Note: Uncomment to use Nuklear.
 

	vkCmdEndRenderPass(g_CommandBuffer);
        vkEndCommandBuffer(g_CommandBuffer);

        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &g_CommandBuffer
        };

        vkQueueSubmit(g_Queue, 1, &submitInfo, VK_NULL_HANDLE);

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

    vkDeviceWaitIdle(g_Device);
}


Application* RoofNutCreate(const ApplicationSpecification* specification) {
    if (!glfwInit()) {
        const char* error_description;
        glfwGetError(&error_description);
        printf("GLFW Initialization failed: %s\n", error_description);
        return NULL;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    Application* app = (Application*)malloc(sizeof(Application));
    if (!app) {
        printf("Allocation of Application failed\n");
        glfwTerminate();
        return NULL;
    }

    app->windowHandle = glfwCreateWindow(specification->width, specification->height, specification->name, NULL, NULL);
    if (!app->windowHandle) {
        printf("Failed to create GLFW window. \n");
        free(app);
        glfwTerminate();
        return NULL;
    }

    g_Window = app->windowHandle;

    return app;
}


void DestroyVulkan() {
    vkDestroySemaphore(g_Device, renderSemaphore, NULL);
    vkDestroySemaphore(g_Device, imageAvailableSemaphore, NULL);
    vkDestroySwapchainKHR(g_Device, g_Swapchain, NULL);
    vkDestroySurfaceKHR(g_Instance, g_Surface, NULL);
    vkDestroyDevice(g_Device, NULL);
    vkDestroyInstance(g_Instance, NULL);

    free(swapchainImages);
    free(imageViews);

  
}


void RoofNutDestroy(Application* app) {
    if (!app) return;
   
    DestroyVulkan();
    free(app);
    glfwDestroyWindow(app->windowHandle);
    glfwTerminate();

    
}
