#include <stdio.h>
#include <roofnut.h>

int main() {
    // Application Specification
    ApplicationSpecification appSpec = {
        .name = APPLICATION_NAME,
        .width = 1280,  // Window width
        .height = 720   // Window height
    };

    // Create Application
    Application* app = Application_Create(&appSpec);
    if (!app) {
        fprintf(stderr, "Failed to create application!\n");
        return EXIT_FAILURE;
    }

    // Initialize Vulkan
    init_vulkan();
    init_device();
    create_render_pass();

    // Initialize Nuklear
    if (nk_glfw_vulkan_init(app->windowHandle, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize Nuklear!\n");
        Application_Destroy(app);
        return -1;
    }

    // Main Loop
    while (!glfwWindowShouldClose(app->windowHandle)) {
        glfwPollEvents();  // Process input events

        // Render GUI with Vulkan
        nk_glfw_vulkan_render(ctx);
    }

    // Cleanup
    Application_Destroy(app);
    cleanup_vulkan();

    return 0;
}
