// All includes statements.
#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// Define Nuklear's setup / include Nuklear.
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_GLFW
#define NK_INCLUDE_VULKAN
#define NK_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_glfw_vulkan.h"

struct nk_context *ctx; 

// Declare the global Vulkan resources.
VkInstance g_Instance;
VkPhysicalDevice g_PhysicalDevice;
VkDevice g_Device;
VkRenderPass g_RenderPass;
VkFramebuffer g_Framebuffer;
VkExtent2D g_SwapChainExtent;
GLFWwindow* g_Window;
VkQueue g_Queue;
VkCommandBuffer g_CommandBuffer; // Vulkan commandbuffer.
VkCommandPool g_CommandPool; // Command pool for Vulkan.
uint32_t queueFamilyIndex = 0;

// Function to check Vulkan results.
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

//Note: What am i writing?
// Initialize Vulkan, GLFW, and Nuklear.
int nk_glfw_vulkan_init(GLFWwindow* window, struct nk_context** outCtx) {
    *outCtx = (struct nk_context*)malloc(sizeof(struct nk_context));
    if (*outCtx == NULL) { // If allocation failed.
        return -1;
    }

    // Note: Nuklear setup can go here. 

    return 0;
}

// Main function to render Nuklear GUI with Vulkan.
void nk_glfw_vulkan_render(struct nk_context* ctx) {
    // Allocate and begin the command buffer.
    VkCommandBuffer commandBuffer = Application_GetCommandBuffer(true);

    // Define the clear color.
    VkClearValue clearColor = {.color = {{0.1f, 0.1f, 0.1f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, // Type of structure (RenderPass)
        .renderPass = g_RenderPass, // The Vulkan RenderPass object.
        .framebuffer = g_Framebuffer, // Specifies the framebuffer. (Where the rendering will take place.)
        .renderArea = {  // Area that will take effect of the render pass.
            .offset = {0, 0},
            .extent = g_SwapChainExtent
        },
        .clearValueCount = 1,  // Number of clear values.
        .pClearValues = &clearColor // The clear values.
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); // Begin the RenderPass with the information from the renderPassInfo struct.

    // Prepare Nuklear buffers
    struct nk_buffer cmds, vertices, elements;
    nk_buffer_init_default(&cmds); // Init the commands.
    nk_buffer_init_default(&vertices); // Init the vertices.
    nk_buffer_init_default(&elements); // Init the elements.

    // Set up Nuklear config.
    struct nk_convert_config config = {
        .global_alpha = 1.0f,
        .shape_AA = NK_ANTI_ALIASING_ON,
        .line_AA = NK_ANTI_ALIASING_ON,
        .circle_segment_count = 22,
        .curve_segment_count = 22,
        .arc_segment_count = 22,
    };

    if (nk_convert(ctx, &cmds, &vertices, &elements, &config) == NK_CONVERT_SUCCESS) { // If convert was successful.
        // Iterate over each draw command
        const struct nk_draw_command* cmd;
        nk_draw_foreach(cmd, ctx, &cmds) {
            if (!cmd->elem_count) continue;  // Skip empty commands

            // Set the scissor rectangle for Vulkan.
            VkRect2D scissorRect = {
                .offset = {(int32_t)cmd->clip_rect.x, (int32_t)cmd->clip_rect.y},
                .extent = {(uint32_t)cmd->clip_rect.w, (uint32_t)cmd->clip_rect.h}
            };
            vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);

            // Draw the Nuklear elements
            vkCmdDraw(commandBuffer, cmd->elem_count, 1, 0, 0);
        }
    }

    // Clean up the temporary buffers.
    nk_buffer_free(&cmds); 
    nk_buffer_free(&vertices);
    nk_buffer_free(&elements);

    vkCmdEndRenderPass(commandBuffer);

    // End and submit the command buffer.
    Application_FlushCommandBuffer(commandBuffer);
}

void nk_glfw_vulkan_shutdown(struct nk_context* ctx) { 
    free(ctx); // Free the Nuklear rendering context (ctx).
}

// Function to create a Vulkan render pass.
void create_render_pass() {
    // Create the attachment description for the color attachment.
    VkAttachmentDescription colorAttachment = {
        .format = VK_FORMAT_B8G8R8A8_UNORM, // Color format for the swapchain.
        .samples = VK_SAMPLE_COUNT_1_BIT,  // Number of samples (1 for no multisampling).
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // Clear the attachment before the render pass starts.
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // Store the attachment after rendering.
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // No stencil buffer operation.
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, // No stencil buffer store operation.
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // Initial layout for the attachment.
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // Final layout for presenting to the swapchain.
    };

    // Create a reference to the color attachment.
    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0, // Index of the attachment (0 for color).
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL // Layout to use for color attachment during rendering.
    };

    // Create a subpass description (this represents the rendering pipeline and its attachments).
    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS, // The pipeline bind point is graphics.
        .colorAttachmentCount = 1, // Only one color attachment.
        .pColorAttachments = &colorAttachmentRef // Reference to the color attachment.
    };

    // Create the render pass creation info structure.
    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, // Type of the structure.
        .attachmentCount = 1, // Number of attachments.
        .pAttachments = &colorAttachment, // Reference to the attachment.
        .subpassCount = 1, // Number of subpasses.
        .pSubpasses = &subpass // Reference to the subpass description.
    };

    // Create the render pass using Vulkan API.
    VkResult res = vkCreateRenderPass(g_Device, &renderPassInfo, NULL, &g_RenderPass);
    check_vk_result(res); // Check the result of render pass creation.
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Make the window non-resizable if you prefer.

    // Create the window
    Application* app = (Application*)malloc(sizeof(Application));
    if (!app)  {
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

    // Initialize Vulkan here if needed, this step can be part of the app setup.

    return app; // Return the app if everything is fine.
}

// Main application shutdown and cleanup function.
void Application_Destroy(Application* app) {
    if (!app) return;

    // Clean up Nuklear and Vulkan resources.
    nk_glfw_vulkan_shutdown(ctx);

    glfwDestroyWindow(app->windowHandle);  // Destroy the GLFW window.
    glfwTerminate();  // Terminate GLFW.
    free(app); // Free the app.
}

// Initialization of Vulkan.
void init_vulkan() {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "RoofNut application.",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "RoofNut",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
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

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, devices);

    if (deviceCount == 0) {
        exit(EXIT_FAILURE); // If failed to find Vulkan device.
    }

    g_PhysicalDevice = devices[0];

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO // Create device info..
    };

    VkResult res = vkCreateDevice(g_PhysicalDevice, &createInfo, NULL, &g_Device);
    check_vk_result(res);  // Check device creation.
}

// Function to retrieve the Vulkan command buffer.
VkCommandBuffer Application_GetCommandBuffer(bool begin) {
    if (g_CommandBuffer == VK_NULL_HANDLE) {
        // For now, just a simple allocation.
        VkCommandBufferAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = g_CommandPool, // Use the command pool here.
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        // Allocate the command buffers.
        VkResult res = vkAllocateCommandBuffers(g_Device, &allocInfo, &g_CommandBuffer);
        if (res != VK_SUCCESS) {
            exit(EXIT_FAILURE);
        }
    }

    if (begin) {
        // Begin recording commands to the command buffer.
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0, // Add relevant flags if necessary
            .pInheritanceInfo = NULL
        };

        VkResult res = vkBeginCommandBuffer(g_CommandBuffer, &beginInfo);
        if (res != VK_SUCCESS) {
            exit(EXIT_FAILURE);
        }
    }

    return g_CommandBuffer;
}

// Function to submit the command buffer and cleanup.
void Application_FlushCommandBuffer(VkCommandBuffer commandBuffer) {
    VkResult res = vkEndCommandBuffer(commandBuffer);
    if (res != VK_SUCCESS) {
        exit(EXIT_FAILURE);
    }

    // Note: Further code needed?
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer
    };

    // Submit the command buffer.
    res = vkQueueSubmit(g_Queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (res != VK_SUCCESS) {
        exit(EXIT_FAILURE);
    }

    // Note: Wait for the GPU to finish?
    vkQueueWaitIdle(g_Queue);

    // After flushing, reset the command buffer for the next use.
    vkFreeCommandBuffers(g_Device, g_CommandPool, 1, &commandBuffer);
}

// Function to clean up Vulkan resources.
void cleanup_vulkan() {
    // First, destroy the render pass.
    if (g_RenderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(g_Device, g_RenderPass, NULL);
        g_RenderPass = VK_NULL_HANDLE; // Reset to null handle after destruction.
    }

    // Destroy the Vulkan device.
    if (g_Device != VK_NULL_HANDLE) {
        vkDestroyDevice(g_Device, NULL);
        g_Device = VK_NULL_HANDLE; // Reset to null handle after destruction.
    }

    // Destroy the Vulkan instance.
    if (g_Instance != VK_NULL_HANDLE) {
        vkDestroyInstance(g_Instance, NULL);
        g_Instance = VK_NULL_HANDLE; // Reset to null handle after destruction.
    }
}


