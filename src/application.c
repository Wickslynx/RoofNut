//All includes statments.
#include "application.h" 
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

//Define and include nuklears setup.
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
struct nk_context *ctx; //Declare the nuklear rendering context.

//Note: I have no idea what im writing.... What even is this??

// Declare the global vulkan resources.
VkInstance g_Instance;
VkPhysicalDevice g_PhysicalDevice;
VkDevice g_Device;
VkRenderPass g_RenderPass;
VkFramebuffer g_Framebuffer;
VkExtent2D g_SwapChainExtent;
GLFWwindow* g_Window;
VkQueue g_Queue;
uint32_t queueFamilyIndex = 0;


// Function to check Vulkan results
void check_vk_result(VkResult err) {
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Vulkan Error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

// Initilaze vulkan, GLFW and nuklear.
int nk_glfw_vulkan_init(GLFWwindow* window, struct nk_context** outCtx) {
    *outCtx = (struct nk_context*)malloc(sizeof(struct nk_context));
    if (*outCtx == NULL) { //If allocation failed.
        return -1;  
    }

    //Note: Nuklear setup can go here. 
    
    return 0;
}

// Main function to render Nuklear GUI with vulkan
void nk_glfw_vulkan_render(struct nk_context* ctx) {
    // Allocate and begin the command buffer.
    VkCommandBuffer commandBuffer = Application_GetCommandBuffer(true);

    // Define the clear color
    VkClearValue clearColor = {.color = {{0.1f, 0.1f, 0.1f, 1.0f}}};

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, //Type of structure (RenderPass)
        .renderPass = g_RenderPass, //The vulkan RenderPass object.
        .framebuffer = g_Framebuffer, //Specifies the framebuffer. (Where the rendering will take place.)
        .renderArea = {  //Area that will take affect of the renderpass.
            .offset = {0, 0},
            .extent = g_SwapChainExtent
        },
        .clearValueCount = 1,  //Number of clear values.
        .pClearValues = &clearColor //The clear values.
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //Begin the RenderPass with the information from the renderPassInfo struct.

    // Prepare Nuklear buffers
    struct nk_buffer cmds, vertices, elements;
    nk_buffer_init_default(&cmds); //Init the commands.
    nk_buffer_init_default(&vertices); //Init the vertecies.
    nk_buffer_init_default(&elements); //Init the elemts.

    // Set up Nuklear config.
    struct nk_convert_config config = {
        .global_alpha = 1.0f,
        .shape_AA = NK_ANTI_ALIASING_ON,
        .line_AA = NK_ANTI_ALIASING_ON,
        .circle_segment_count = 22,
        .curve_segment_count = 22,
        .arc_segment_count = 22,
    };
    
    if (nk_convert(ctx, &cmds, &vertices, &elements, &config) == NK_CONVERT_SUCCESS) { //If convert was succesfull.
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

// Main application function, used to create the application.
Application* Application_Create(const ApplicationSpecification* specification) {
    // Allocate the application, Initialize members.
    Application* app = (Application*)malloc(sizeof(Application));
    
    // Initialize GLFW, Nuklear, and other stuff...
    if (nk_glfw_vulkan_init(app->windowHandle, &ctx) != 0) {
        printf("Failed to initialize Nuklear.\n"); //If not succesfull, print error.
        free(app);
        return NULL;
    }

    return app;
}

// Main application shutdown and cleanup func.
void Application_Destroy(Application* app) {
    if (!app) return;

    // Clean up Nuklear and Vulkan resources
    nk_glfw_vulkan_shutdown(ctx);

    glfwDestroyWindow(app->windowHandle);  // Destroy the GLFW window
    glfwTerminate();  // Terminate GLFW
    free(app);
}

// Initialization of Vulkan
void init_vulkan() {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "RoofNut Application",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo
    };

    VkResult res = vkCreateInstance(&createInfo, NULL, &g_Instance);
    check_vk_result(res);  // Check instance creation
}

// Initialization of device
void init_device() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, NULL);

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(g_Instance, &deviceCount, devices);

    if (deviceCount == 0) {
        fprintf(stderr, "Failed to find a GPU with Vulkan support!\n");
        exit(EXIT_FAILURE);
    }

    g_PhysicalDevice = devices[0];

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO
    };

    VkResult res = vkCreateDevice(g_PhysicalDevice, &createInfo, NULL, &g_Device);
    check_vk_result(res);  // Check device creation
}

// Creating render pass
void create_render_pass() {
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
    check_vk_result(res);  // Check render pass creation
}

// Clean up Vulkan resources
void cleanup_vulkan() {
    vkDestroyRenderPass(g_Device, g_RenderPass, NULL);
    vkDestroyDevice(g_Device, NULL);
    vkDestroyInstance(g_Instance, NULL);
}

//Note: Other functions can go under here: