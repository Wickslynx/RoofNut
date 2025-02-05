#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include "external/glfw/include/GLFW/glfw3.h"

// Define constants, if needed, for Vulkan or GLFW initialization
#define APPLICATION_NAME "RoofNut Application"

// Vulkan global resources
extern VkInstance g_Instance;
extern VkPhysicalDevice g_PhysicalDevice;
extern VkDevice g_Device;
extern VkRenderPass g_RenderPass;
extern VkFramebuffer g_Framebuffer;
extern VkExtent2D g_SwapChainExtent;
extern GLFWwindow* g_Window;
extern VkQueue g_Queue;
extern uint32_t queueFamilyIndex;
extern VkCommandBuffer g_CommandBuffer; // Command buffer for Vulkan

// Nuklear context
struct nk_context *ctx; // Nuklear context for GUI

 //Struct declarations.
typedef struct {
    const char* name;   // Application name
    uint32_t width;     // Application window width
    uint32_t height;    // Application window height
} ApplicationSpecification;

typedef void (*LayerCallback)(void);

typedef struct {
    ApplicationSpecification specification;
    GLFWwindow* windowHandle;
    bool running;
    bool customTitleBar;
    size_t layerCount;
    struct Layer** layers;
    LayerCallback menubarCallback;  // Function pointer for menubar callback
} Application;


// Function declarations

// Vulkan utilities
void check_vk_result(VkResult err);
void init_vulkan();
void init_device();
void create_render_pass();
void cleanup_vulkan();

// Vulkan Command Buffer
VkCommandBuffer Application_GetCommandBuffer(bool begin);
void Application_FlushCommandBuffer(VkCommandBuffer commandBuffer);

// Application lifecycle functions
Application* Application_Create(const ApplicationSpecification* specification);
void Application_Destroy(Application* app);

void RoofNutRender();
// Nuklear and GLFW Vulkan integration
int nk_glfw_vulkan_init(GLFWwindow* window, struct nk_context** outCtx);
void nk_glfw_vulkan_render(struct nk_context* ctx);
void nk_glfw_vulkan_shutdown(struct nk_context* ctx);
void create_render_pass(void);
void cleanup_vulkan(void);

enum nk_anti_aliasing;



#endif // APPLICATION_H
